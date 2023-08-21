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
        .left {
            display:    block;
            overflow-y: scroll;
            width:      600px;
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
        table.enumeration {
            border:             1px solid LightGrey;
            border-collapse:    collapse;
            width:              100%;
            font-size:          90%;
        }
        .item td:first-child {
            width:              33%;
            white-space:        nowrap;
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
<h1>Module gui</h1>
<h2>Structs</h2>
<h3></h3>
<ul>
<li><a href="#Gui_ActionContext">ActionContext</a></li>
<li><a href="#Gui_Application">Application</a></li>
<li><a href="#Gui_ApplicationNative">ApplicationNative</a></li>
<li><a href="#Gui_CommandEvent">CommandEvent</a></li>
<li><a href="#Gui_CommandStateEvent">CommandStateEvent</a></li>
<li><a href="#Gui_CreateEvent">CreateEvent</a></li>
<li><a href="#Gui_DestroyEvent">DestroyEvent</a></li>
<li><a href="#Gui_Event">Event</a></li>
<li><a href="#Gui_FocusEvent">FocusEvent</a></li>
<li><a href="#Gui_FrameEvent">FrameEvent</a></li>
<li><a href="#Gui_Item">Item</a></li>
<li><a href="#Gui_KeyEvent">KeyEvent</a></li>
<li><a href="#Gui_MouseEvent">MouseEvent</a></li>
<li><a href="#Gui_MoveEvent">MoveEvent</a></li>
<li><a href="#Gui_Movie">Movie</a></li>
<li><a href="#Gui_NativeSurface">NativeSurface</a></li>
<li><a href="#Gui_NotifyEvent">NotifyEvent</a></li>
<li><a href="#Gui_PaintEvent">PaintEvent</a></li>
<li><a href="#Gui_QuitEvent">QuitEvent</a></li>
<li><a href="#Gui_ResizeEvent">ResizeEvent</a></li>
<li><a href="#Gui_SelModel">SelModel</a></li>
<li><a href="#Gui_SerializeStateEvent">SerializeStateEvent</a></li>
<li><a href="#Gui_SetThemeEvent">SetThemeEvent</a></li>
<li><a href="#Gui_SigArray">SigArray</a></li>
<li><a href="#Gui_StateEvent">StateEvent</a></li>
<li><a href="#Gui_Surface">Surface</a></li>
<li><a href="#Gui_SurfaceState">SurfaceState</a></li>
<li><a href="#Gui_SysCommandEvent">SysCommandEvent</a></li>
<li><a href="#Gui_SysUserEvent">SysUserEvent</a></li>
<li><a href="#Gui_Timer">Timer</a></li>
<li><a href="#Gui_TimerEvent">TimerEvent</a></li>
</ul>
<h3>composite</h3>
<ul>
<li><a href="#Gui_ColorPickerCtrl">ColorPickerCtrl</a></li>
<li><a href="#Gui_ComboCtrl">ComboCtrl</a></li>
<li><a href="#Gui_EditCtrl">EditCtrl</a></li>
<li><a href="#Gui_EmbInfoCtrl">EmbInfoCtrl</a></li>
<li><a href="#Gui_GridLayoutCtrl">GridLayoutCtrl</a></li>
<li><a href="#Gui_ListCtrl">ListCtrl</a></li>
<li><a href="#Gui_ListItem">ListItem</a></li>
<li><a href="#Gui_ListLine">ListLine</a></li>
<li><a href="#Gui_ListView">ListView</a></li>
<li><a href="#Gui_MenuCtrl">MenuCtrl</a></li>
<li><a href="#Gui_PopupListCtrl">PopupListCtrl</a></li>
<li><a href="#Gui_PopupListCtrlItem">PopupListCtrlItem</a></li>
<li><a href="#Gui_PopupListView">PopupListView</a></li>
<li><a href="#Gui_PopupMenuItem">PopupMenuItem</a></li>
<li><a href="#Gui_SliderCtrl">SliderCtrl</a></li>
<li><a href="#Gui_SplitterCtrl">SplitterCtrl</a></li>
<li><a href="#Gui_SplitterItem">SplitterItem</a></li>
<li><a href="#Gui_StackLayoutCtrl">StackLayoutCtrl</a></li>
<li><a href="#Gui_WrapLayoutCtrl">WrapLayoutCtrl</a></li>
</ul>
<h3>dialogs</h3>
<ul>
<li><a href="#Gui_Dialog">Dialog</a></li>
<li><a href="#Gui_EditDlg">EditDlg</a></li>
<li><a href="#Gui_FileDlg">FileDlg</a></li>
<li><a href="#Gui_FileDlgOptions">FileDlgOptions</a></li>
<li><a href="#Gui_FileDlgState">FileDlgState</a></li>
<li><a href="#Gui_MessageDlg">MessageDlg</a></li>
</ul>
<h3>paint</h3>
<ul>
<li><a href="#Gui_BlendColor">BlendColor</a></li>
<li><a href="#Gui_Cursor">Cursor</a></li>
<li><a href="#Gui_Icon">Icon</a></li>
<li><a href="#Gui_ImageList">ImageList</a></li>
<li><a href="#Gui_NativeCursor">NativeCursor</a></li>
<li><a href="#Gui_PaintContext">PaintContext</a></li>
<li><a href="#Gui_PaintImage">PaintImage</a></li>
<li><a href="#Gui_Theme">Theme</a></li>
<li><a href="#Gui_ThemeColors">ThemeColors</a></li>
<li><a href="#Gui_ThemeImageRect">ThemeImageRect</a></li>
<li><a href="#Gui_ThemeImageRects">ThemeImageRects</a></li>
<li><a href="#Gui_ThemeMetrics">ThemeMetrics</a></li>
<li><a href="#Gui_ThemeResources">ThemeResources</a></li>
<li><a href="#Gui_ThemeStyle">ThemeStyle</a></li>
<li><a href="#Gui_ThemeStyleRef">ThemeStyleRef</a></li>
</ul>
<h3>property</h3>
<ul>
<li><a href="#Gui_PropertyList">PropertyList</a></li>
<li><a href="#Gui_PropertyListCtrl">PropertyListCtrl</a></li>
<li><a href="#Gui_PropertyListItem">PropertyListItem</a></li>
<li><a href="#Gui_PropertyListView">PropertyListView</a></li>
</ul>
<h3>richedit</h3>
<ul>
<li><a href="#Gui_RichEditCtrl">RichEditCtrl</a></li>
<li><a href="#Gui_RichEditCursor">RichEditCursor</a></li>
<li><a href="#Gui_RichEditLexerSwag">RichEditLexerSwag</a></li>
<li><a href="#Gui_RichEditLine">RichEditLine</a></li>
<li><a href="#Gui_RichEditMapping">RichEditMapping</a></li>
<li><a href="#Gui_RichEditRuneStyle">RichEditRuneStyle</a></li>
<li><a href="#Gui_RichEditUndo">RichEditUndo</a></li>
<li><a href="#Gui_RichEditView">RichEditView</a></li>
</ul>
<h3>widgets</h3>
<ul>
<li><a href="#Gui_Button">Button</a></li>
<li><a href="#Gui_CheckButton">CheckButton</a></li>
<li><a href="#Gui_ColorPicker">ColorPicker</a></li>
<li><a href="#Gui_ComboBox">ComboBox</a></li>
<li><a href="#Gui_ComboBoxItem">ComboBoxItem</a></li>
<li><a href="#Gui_EditBox">EditBox</a></li>
<li><a href="#Gui_Header">Header</a></li>
<li><a href="#Gui_HeaderItem">HeaderItem</a></li>
<li><a href="#Gui_IconBar">IconBar</a></li>
<li><a href="#Gui_IconButton">IconButton</a></li>
<li><a href="#Gui_ImageRect">ImageRect</a></li>
<li><a href="#Gui_Label">Label</a></li>
<li><a href="#Gui_PalettePicker">PalettePicker</a></li>
<li><a href="#Gui_ProgressBar">ProgressBar</a></li>
<li><a href="#Gui_PushButton">PushButton</a></li>
<li><a href="#Gui_RadioButton">RadioButton</a></li>
<li><a href="#Gui_Slider">Slider</a></li>
<li><a href="#Gui_Tab">Tab</a></li>
<li><a href="#Gui_TabItem">TabItem</a></li>
<li><a href="#Gui_ToggleButton">ToggleButton</a></li>
</ul>
<h3>wnd</h3>
<ul>
<li><a href="#Gui_FrameWnd">FrameWnd</a></li>
<li><a href="#Gui_KeyShortcut">KeyShortcut</a></li>
<li><a href="#Gui_ScrollWnd">ScrollWnd</a></li>
<li><a href="#Gui_SurfaceWnd">SurfaceWnd</a></li>
<li><a href="#Gui_Wnd">Wnd</a></li>
</ul>
<h2>Interfaces</h2>
<h3></h3>
<ul>
<li><a href="#Gui_IActionUI">IActionUI</a></li>
</ul>
<h3>richedit</h3>
<ul>
<li><a href="#Gui_IRichEditLexer">IRichEditLexer</a></li>
</ul>
<h3>wnd</h3>
<ul>
<li><a href="#Gui_IWnd">IWnd</a></li>
</ul>
<h2>Enums</h2>
<h3></h3>
<ul>
<li><a href="#Gui_EventKind">EventKind</a></li>
<li><a href="#Gui_MovieSource">MovieSource</a></li>
<li><a href="#Gui_SurfaceFlags">SurfaceFlags</a></li>
</ul>
<h3>composite</h3>
<ul>
<li><a href="#Gui_ColorPickerViewMode">ColorPickerViewMode</a></li>
<li><a href="#Gui_EmbInfoCtrlKind">EmbInfoCtrlKind</a></li>
<li><a href="#Gui_GridLayoutCtrlFlags">GridLayoutCtrlFlags</a></li>
<li><a href="#Gui_ListFlags">ListFlags</a></li>
<li><a href="#Gui_ListSelectionMode">ListSelectionMode</a></li>
<li><a href="#Gui_MenuCtrlFlags">MenuCtrlFlags</a></li>
<li><a href="#Gui_PopupPos">PopupPos</a></li>
<li><a href="#Gui_SplitterFlags">SplitterFlags</a></li>
<li><a href="#Gui_StackLayoutKind">StackLayoutKind</a></li>
</ul>
<h3>dialogs</h3>
<ul>
<li><a href="#Gui_FileDlgMode">FileDlgMode</a></li>
</ul>
<h3>paint</h3>
<ul>
<li><a href="#Gui_CursorShape">CursorShape</a></li>
<li><a href="#Gui_ThemeIcons24">ThemeIcons24</a></li>
<li><a href="#Gui_ThemeIcons64">ThemeIcons64</a></li>
</ul>
<h3>richedit</h3>
<ul>
<li><a href="#Gui_RichEditCommand">RichEditCommand</a></li>
<li><a href="#Gui_RichEditFlags">RichEditFlags</a></li>
<li><a href="#Gui_RichEditForm">RichEditForm</a></li>
<li><a href="#Gui_RichEditUndoType">RichEditUndoType</a></li>
</ul>
<h3>widgets</h3>
<ul>
<li><a href="#Gui_ButtonCheckState">ButtonCheckState</a></li>
<li><a href="#Gui_CheckButtonFlags">CheckButtonFlags</a></li>
<li><a href="#Gui_ColorPickerMode">ColorPickerMode</a></li>
<li><a href="#Gui_ComboBoxKind">ComboBoxKind</a></li>
<li><a href="#Gui_EditBoxCheckResult">EditBoxCheckResult</a></li>
<li><a href="#Gui_EditBoxFlags">EditBoxFlags</a></li>
<li><a href="#Gui_EditBoxForm">EditBoxForm</a></li>
<li><a href="#Gui_EditBoxInputMode">EditBoxInputMode</a></li>
<li><a href="#Gui_HeaderFlags">HeaderFlags</a></li>
<li><a href="#Gui_HeaderForm">HeaderForm</a></li>
<li><a href="#Gui_IconBarFlags">IconBarFlags</a></li>
<li><a href="#Gui_IconButtonArrowPos">IconButtonArrowPos</a></li>
<li><a href="#Gui_IconButtonCheckedForm">IconButtonCheckedForm</a></li>
<li><a href="#Gui_IconButtonFlags">IconButtonFlags</a></li>
<li><a href="#Gui_IconButtonForm">IconButtonForm</a></li>
<li><a href="#Gui_IconButtonTextPos">IconButtonTextPos</a></li>
<li><a href="#Gui_LabelFlags">LabelFlags</a></li>
<li><a href="#Gui_ProgressBarFlags">ProgressBarFlags</a></li>
<li><a href="#Gui_PushButtonForm">PushButtonForm</a></li>
<li><a href="#Gui_RadioButtonFlags">RadioButtonFlags</a></li>
<li><a href="#Gui_SliderFlags">SliderFlags</a></li>
<li><a href="#Gui_TabBarForm">TabBarForm</a></li>
<li><a href="#Gui_TabBarLayout">TabBarLayout</a></li>
<li><a href="#Gui_TabForm">TabForm</a></li>
<li><a href="#Gui_TabItemForm">TabItemForm</a></li>
<li><a href="#Gui_TabViewForm">TabViewForm</a></li>
</ul>
<h3>wnd</h3>
<ul>
<li><a href="#Gui_AnchorStyle">AnchorStyle</a></li>
<li><a href="#Gui_BackgroundStyle">BackgroundStyle</a></li>
<li><a href="#Gui_DockStyle">DockStyle</a></li>
<li><a href="#Gui_FocusStategy">FocusStategy</a></li>
<li><a href="#Gui_FrameWndAnchor">FrameWndAnchor</a></li>
<li><a href="#Gui_FrameWndFlags">FrameWndFlags</a></li>
<li><a href="#Gui_FrameWndForm">FrameWndForm</a></li>
<li><a href="#Gui_ScrollWndFlags">ScrollWndFlags</a></li>
<li><a href="#Gui_SizingBorder">SizingBorder</a></li>
<li><a href="#Gui_WndFlags">WndFlags</a></li>
</ul>
<h2>Attributes</h2>
<h3>property</h3>
<ul>
<li><a href="#Gui_PropertyList_Category">Category</a></li>
<li><a href="#Gui_PropertyList_Description">Description</a></li>
<li><a href="#Gui_PropertyList_EditSlider">EditSlider</a></li>
<li><a href="#Gui_PropertyList_Name">Name</a></li>
<li><a href="#Gui_PropertyList_SubCategory">SubCategory</a></li>
</ul>
<h2>Functions</h2>
<h3></h3>
<ul>
<li><a href="#Gui_Application_addTimer">Application.addTimer</a></li>
<li><a href="#Gui_Application_createSurface">Application.createSurface</a></li>
<li><a href="#Gui_Application_deleteTimer">Application.deleteTimer</a></li>
<li><a href="#Gui_Application_destroyWnd">Application.destroyWnd</a></li>
<li><a href="#Gui_Application_doModalLoop">Application.doModalLoop</a></li>
<li><a href="#Gui_Application_exitModal">Application.exitModal</a></li>
<li><a href="#Gui_Application_getDirectoryIcon">Application.getDirectoryIcon</a></li>
<li><a href="#Gui_Application_getDt">Application.getDt</a></li>
<li><a href="#Gui_Application_getFileIcon">Application.getFileIcon</a></li>
<li><a href="#Gui_Application_getHotKeyShortcut">Application.getHotKeyShortcut</a></li>
<li><a href="#Gui_Application_getHotKeyShortcutNameFor">Application.getHotKeyShortcutNameFor</a></li>
<li><a href="#Gui_Application_getKeyboard">Application.getKeyboard</a></li>
<li><a href="#Gui_Application_getMouse">Application.getMouse</a></li>
<li><a href="#Gui_Application_getMouseCapture">Application.getMouseCapture</a></li>
<li><a href="#Gui_Application_getRenderer">Application.getRenderer</a></li>
<li><a href="#Gui_Application_getWndAtMouse">Application.getWndAtMouse</a></li>
<li><a href="#Gui_Application_invalidate">Application.invalidate</a></li>
<li><a href="#Gui_Application_loadState">Application.loadState</a></li>
<li><a href="#Gui_Application_popCursor">Application.popCursor</a></li>
<li><a href="#Gui_Application_postEvent">Application.postEvent</a></li>
<li><a href="#Gui_Application_postQuitEvent">Application.postQuitEvent</a></li>
<li><a href="#Gui_Application_pushCursor">Application.pushCursor</a></li>
<li><a href="#Gui_Application_registerFrameEvent">Application.registerFrameEvent</a></li>
<li><a href="#Gui_Application_registerHookEvents">Application.registerHookEvents</a></li>
<li><a href="#Gui_Application_registerHotKey">Application.registerHotKey</a></li>
<li><a href="#Gui_Application_run">Application.run</a></li>
<li><a href="#Gui_Application_runFrame">Application.runFrame</a></li>
<li><a href="#Gui_Application_runSurface">Application.runSurface</a></li>
<li><a href="#Gui_Application_saveState">Application.saveState</a></li>
<li><a href="#Gui_Application_sendEvent">Application.sendEvent</a></li>
<li><a href="#Gui_Application_setActivated">Application.setActivated</a></li>
<li><a href="#Gui_Application_setAppIcon">Application.setAppIcon</a></li>
<li><a href="#Gui_Application_setConfigPath">Application.setConfigPath</a></li>
<li><a href="#Gui_Application_unregisterFrameEvent">Application.unregisterFrameEvent</a></li>
<li><a href="#Gui_Application_unregisterHookEvents">Application.unregisterHookEvents</a></li>
<li><a href="#Gui_Clipboard_addData">Clipboard.addData</a></li>
<li><a href="#Gui_Clipboard_addImage">Clipboard.addImage</a></li>
<li><a href="#Gui_Clipboard_addString">Clipboard.addString</a></li>
<li><a href="#Gui_Clipboard_getData">Clipboard.getData</a></li>
<li><a href="#Gui_Clipboard_getImage">Clipboard.getImage</a></li>
<li><a href="#Gui_Clipboard_getString">Clipboard.getString</a></li>
<li><a href="#Gui_Clipboard_hasFormat">Clipboard.hasFormat</a></li>
<li><a href="#Gui_Clipboard_hasImage">Clipboard.hasImage</a></li>
<li><a href="#Gui_Clipboard_hasString">Clipboard.hasString</a></li>
<li><a href="#Gui_Clipboard_registerFormat">Clipboard.registerFormat</a></li>
<li><a href="#Gui_Event_create">Event.create</a></li>
<li><a href="#Gui_DwmExtendFrameIntoClientArea">Gui.DwmExtendFrameIntoClientArea</a></li>
<li><a href="#Gui_Movie_clear">Movie.clear</a></li>
<li><a href="#Gui_Movie_isValid">Movie.isValid</a></li>
<li><a href="#Gui_Movie_pause">Movie.pause</a></li>
<li><a href="#Gui_Movie_set">Movie.set</a></li>
<li><a href="#Gui_Movie_setFrameIndex">Movie.setFrameIndex</a></li>
<li><a href="#Gui_Movie_update">Movie.update</a></li>
<li><a href="#Gui_SelModel_getSelectedRangeIndex">SelModel.getSelectedRangeIndex</a></li>
<li><a href="#Gui_SelModel_isEmpty">SelModel.isEmpty</a></li>
<li><a href="#Gui_SelModel_isSelected">SelModel.isSelected</a></li>
<li><a href="#Gui_SelModel_opVisit">SelModel.opVisit</a></li>
<li><a href="#Gui_SelModel_select">SelModel.select</a></li>
<li><a href="#Gui_SelModel_unselectAll">SelModel.unselectAll</a></li>
<li><a href="#Gui_SigArray_call">SigArray.call</a></li>
<li><a href="#Gui_SigArray_disable">SigArray.disable</a></li>
<li><a href="#Gui_SigArray_enable">SigArray.enable</a></li>
<li><a href="#Gui_SigArray_opAssign">SigArray.opAssign</a></li>
<li><a href="#Gui_Surface_bringToTop">Surface.bringToTop</a></li>
<li><a href="#Gui_Surface_center">Surface.center</a></li>
<li><a href="#Gui_Surface_constaintToScreen">Surface.constaintToScreen</a></li>
<li><a href="#Gui_Surface_create">Surface.create</a></li>
<li><a href="#Gui_Surface_destroy">Surface.destroy</a></li>
<li><a href="#Gui_Surface_disable">Surface.disable</a></li>
<li><a href="#Gui_Surface_enable">Surface.enable</a></li>
<li><a href="#Gui_Surface_getView">Surface.getView</a></li>
<li><a href="#Gui_Surface_hide">Surface.hide</a></li>
<li><a href="#Gui_Surface_invalidate">Surface.invalidate</a></li>
<li><a href="#Gui_Surface_invalidateRect">Surface.invalidateRect</a></li>
<li><a href="#Gui_Surface_paint">Surface.paint</a></li>
<li><a href="#Gui_Surface_screenToSurface">Surface.screenToSurface</a></li>
<li><a href="#Gui_Surface_setIcon">Surface.setIcon</a></li>
<li><a href="#Gui_Surface_setPosition">Surface.setPosition</a></li>
<li><a href="#Gui_Surface_setSurfaceFlags">Surface.setSurfaceFlags</a></li>
<li><a href="#Gui_Surface_setTitle">Surface.setTitle</a></li>
<li><a href="#Gui_Surface_setView">Surface.setView</a></li>
<li><a href="#Gui_Surface_show">Surface.show</a></li>
<li><a href="#Gui_Surface_showMaximized">Surface.showMaximized</a></li>
<li><a href="#Gui_Surface_showMinimized">Surface.showMinimized</a></li>
<li><a href="#Gui_Surface_showNormal">Surface.showNormal</a></li>
<li><a href="#Gui_Surface_surfaceToScreen">Surface.surfaceToScreen</a></li>
<li><a href="#Gui_Timer_restart">Timer.restart</a></li>
<li><a href="#Gui_ToolTip_hide">ToolTip.hide</a></li>
<li><a href="#Gui_ToolTip_show">ToolTip.show</a></li>
</ul>
<h3>composite</h3>
<ul>
<li><a href="#Gui_ColorPickerCtrl_IWnd_onPaintEvent">ColorPickerCtrl.IWnd.onPaintEvent</a></li>
<li><a href="#Gui_ColorPickerCtrl_create">ColorPickerCtrl.create</a></li>
<li><a href="#Gui_ColorPickerCtrl_getColor">ColorPickerCtrl.getColor</a></li>
<li><a href="#Gui_ColorPickerCtrl_setColor">ColorPickerCtrl.setColor</a></li>
<li><a href="#Gui_ColorPickerCtrl_setMode">ColorPickerCtrl.setMode</a></li>
<li><a href="#Gui_ComboCtrl_IWnd_onResizeEvent">ComboCtrl.IWnd.onResizeEvent</a></li>
<li><a href="#Gui_ComboCtrl_create">ComboCtrl.create</a></li>
<li><a href="#Gui_EditCtrl_IWnd_onResizeEvent">EditCtrl.IWnd.onResizeEvent</a></li>
<li><a href="#Gui_EditCtrl_create">EditCtrl.create</a></li>
<li><a href="#Gui_EmbInfoCtrl_create">EmbInfoCtrl.create</a></li>
<li><a href="#Gui_GridLayoutCtrl_IWnd_onResizeEvent">GridLayoutCtrl.IWnd.onResizeEvent</a></li>
<li><a href="#Gui_GridLayoutCtrl_computeLayout">GridLayoutCtrl.computeLayout</a></li>
<li><a href="#Gui_GridLayoutCtrl_create">GridLayoutCtrl.create</a></li>
<li><a href="#Gui_GridLayoutCtrl_setColRowChild">GridLayoutCtrl.setColRowChild</a></li>
<li><a href="#Gui_GridLayoutCtrl_setColSize">GridLayoutCtrl.setColSize</a></li>
<li><a href="#Gui_GridLayoutCtrl_setRowSize">GridLayoutCtrl.setRowSize</a></li>
<li><a href="#Gui_PopupListView_IWnd_onFocusEvent">IWnd.onFocusEvent</a></li>
<li><a href="#Gui_ListView_IWnd_onKeyEvent">IWnd.onKeyEvent</a></li>
<li><a href="#Gui_SplitterCtrl_IWnd_onPostPaintEvent">IWnd.onPostPaintEvent</a></li>
<li><a href="#Gui_MenuCtrl_IWnd_onTimerEvent">IWnd.onTimerEvent</a></li>
<li><a href="#Gui_ListCtrl_addColumn">ListCtrl.addColumn</a></li>
<li><a href="#Gui_ListCtrl_clear">ListCtrl.clear</a></li>
<li><a href="#Gui_ListCtrl_collapse">ListCtrl.collapse</a></li>
<li><a href="#Gui_ListCtrl_collapseAll">ListCtrl.collapseAll</a></li>
<li><a href="#Gui_ListCtrl_computeLayout">ListCtrl.computeLayout</a></li>
<li><a href="#Gui_ListCtrl_createLine">ListCtrl.createLine</a></li>
<li><a href="#Gui_ListCtrl_createMultiColumns">ListCtrl.createMultiColumns</a></li>
<li><a href="#Gui_ListCtrl_createSimple">ListCtrl.createSimple</a></li>
<li><a href="#Gui_ListCtrl_ensureVisibleLine">ListCtrl.ensureVisibleLine</a></li>
<li><a href="#Gui_ListCtrl_expand">ListCtrl.expand</a></li>
<li><a href="#Gui_ListCtrl_getCheckedLines">ListCtrl.getCheckedLines</a></li>
<li><a href="#Gui_ListCtrl_getFirstVisibleLineIndex">ListCtrl.getFirstVisibleLineIndex</a></li>
<li><a href="#Gui_ListCtrl_getFocusLine">ListCtrl.getFocusLine</a></li>
<li><a href="#Gui_ListCtrl_getLastVisibleLineIndex">ListCtrl.getLastVisibleLineIndex</a></li>
<li><a href="#Gui_ListCtrl_getLine">ListCtrl.getLine</a></li>
<li><a href="#Gui_ListCtrl_getLineCount">ListCtrl.getLineCount</a></li>
<li><a href="#Gui_ListCtrl_getLineHeight">ListCtrl.getLineHeight</a></li>
<li><a href="#Gui_ListCtrl_getSelectedLine">ListCtrl.getSelectedLine</a></li>
<li><a href="#Gui_ListCtrl_getSelectedLines">ListCtrl.getSelectedLines</a></li>
<li><a href="#Gui_ListCtrl_isLineSelected">ListCtrl.isLineSelected</a></li>
<li><a href="#Gui_ListCtrl_isSelectedLine">ListCtrl.isSelectedLine</a></li>
<li><a href="#Gui_ListCtrl_selectLine">ListCtrl.selectLine</a></li>
<li><a href="#Gui_ListCtrl_selectLines">ListCtrl.selectLines</a></li>
<li><a href="#Gui_ListCtrl_setColumnWidth">ListCtrl.setColumnWidth</a></li>
<li><a href="#Gui_ListCtrl_setFocus">ListCtrl.setFocus</a></li>
<li><a href="#Gui_ListCtrl_setVirtualCount">ListCtrl.setVirtualCount</a></li>
<li><a href="#Gui_ListCtrl_sort">ListCtrl.sort</a></li>
<li><a href="#Gui_ListCtrl_toggleExpand">ListCtrl.toggleExpand</a></li>
<li><a href="#Gui_ListCtrl_unselectAll">ListCtrl.unselectAll</a></li>
<li><a href="#Gui_ListLine_canExpand">ListLine.canExpand</a></li>
<li><a href="#Gui_ListLine_getItem">ListLine.getItem</a></li>
<li><a href="#Gui_ListLine_isParentOf">ListLine.isParentOf</a></li>
<li><a href="#Gui_ListLine_setItem">ListLine.setItem</a></li>
<li><a href="#Gui_ListView_IWnd_onMouseEvent">ListView.IWnd.onMouseEvent</a></li>
<li><a href="#Gui_ListView_IWnd_onPaintEvent">ListView.IWnd.onPaintEvent</a></li>
<li><a href="#Gui_ListView_IWnd_onResizeEvent">ListView.IWnd.onResizeEvent</a></li>
<li><a href="#Gui_MenuCtrl_IWnd_onMouseEvent">MenuCtrl.IWnd.onMouseEvent</a></li>
<li><a href="#Gui_MenuCtrl_IWnd_onPaintEvent">MenuCtrl.IWnd.onPaintEvent</a></li>
<li><a href="#Gui_MenuCtrl_addItem">MenuCtrl.addItem</a></li>
<li><a href="#Gui_MenuCtrl_addPopup">MenuCtrl.addPopup</a></li>
<li><a href="#Gui_MenuCtrl_addSeparator">MenuCtrl.addSeparator</a></li>
<li><a href="#Gui_MenuCtrl_computeLayoutPopup">MenuCtrl.computeLayoutPopup</a></li>
<li><a href="#Gui_MenuCtrl_createBar">MenuCtrl.createBar</a></li>
<li><a href="#Gui_MenuCtrl_createPopup">MenuCtrl.createPopup</a></li>
<li><a href="#Gui_MenuCtrl_doModal">MenuCtrl.doModal</a></li>
<li><a href="#Gui_MenuCtrl_updateState">MenuCtrl.updateState</a></li>
<li><a href="#Gui_PopupListCtrl_addItem">PopupListCtrl.addItem</a></li>
<li><a href="#Gui_PopupListCtrl_addSeparator">PopupListCtrl.addSeparator</a></li>
<li><a href="#Gui_PopupListCtrl_create">PopupListCtrl.create</a></li>
<li><a href="#Gui_PopupListCtrl_show">PopupListCtrl.show</a></li>
<li><a href="#Gui_PopupListView_IWnd_onMouseEvent">PopupListView.IWnd.onMouseEvent</a></li>
<li><a href="#Gui_PopupListView_IWnd_onPaintEvent">PopupListView.IWnd.onPaintEvent</a></li>
<li><a href="#Gui_SliderCtrl_IWnd_onResizeEvent">SliderCtrl.IWnd.onResizeEvent</a></li>
<li><a href="#Gui_SliderCtrl_create">SliderCtrl.create</a></li>
<li><a href="#Gui_SliderCtrl_setMinMaxF32">SliderCtrl.setMinMaxF32</a></li>
<li><a href="#Gui_SliderCtrl_setMinMaxS64">SliderCtrl.setMinMaxS64</a></li>
<li><a href="#Gui_SliderCtrl_setMinMaxU64">SliderCtrl.setMinMaxU64</a></li>
<li><a href="#Gui_SliderCtrl_setValue">SliderCtrl.setValue</a></li>
<li><a href="#Gui_SplitterCtrl_IWnd_onMouseEvent">SplitterCtrl.IWnd.onMouseEvent</a></li>
<li><a href="#Gui_SplitterCtrl_IWnd_onResizeEvent">SplitterCtrl.IWnd.onResizeEvent</a></li>
<li><a href="#Gui_SplitterCtrl_addPane">SplitterCtrl.addPane</a></li>
<li><a href="#Gui_SplitterCtrl_create">SplitterCtrl.create</a></li>
<li><a href="#Gui_SplitterCtrl_isVertical">SplitterCtrl.isVertical</a></li>
<li><a href="#Gui_SplitterCtrl_setPaneSize">SplitterCtrl.setPaneSize</a></li>
<li><a href="#Gui_StackLayoutCtrl_IWnd_onResizeEvent">StackLayoutCtrl.IWnd.onResizeEvent</a></li>
<li><a href="#Gui_StackLayoutCtrl_computeLayout">StackLayoutCtrl.computeLayout</a></li>
<li><a href="#Gui_StackLayoutCtrl_create">StackLayoutCtrl.create</a></li>
<li><a href="#Gui_WrapLayoutCtrl_IWnd_onResizeEvent">WrapLayoutCtrl.IWnd.onResizeEvent</a></li>
<li><a href="#Gui_WrapLayoutCtrl_computeLayout">WrapLayoutCtrl.computeLayout</a></li>
<li><a href="#Gui_WrapLayoutCtrl_create">WrapLayoutCtrl.create</a></li>
</ul>
<h3>dialogs</h3>
<ul>
<li><a href="#Gui_Dialog_addButton">Dialog.addButton</a></li>
<li><a href="#Gui_Dialog_createDialog">Dialog.createDialog</a></li>
<li><a href="#Gui_Dialog_createSurface">Dialog.createSurface</a></li>
<li><a href="#Gui_Dialog_doModal">Dialog.doModal</a></li>
<li><a href="#Gui_Dialog_validateId">Dialog.validateId</a></li>
<li><a href="#Gui_EditDlg_createMultiLine">EditDlg.createMultiLine</a></li>
<li><a href="#Gui_EditDlg_createSingleLine">EditDlg.createSingleLine</a></li>
<li><a href="#Gui_EditDlg_getText">EditDlg.getText</a></li>
<li><a href="#Gui_EditDlg_setMessage">EditDlg.setMessage</a></li>
<li><a href="#Gui_EditDlg_setText">EditDlg.setText</a></li>
<li><a href="#Gui_FileDlg_create">FileDlg.create</a></li>
<li><a href="#Gui_FileDlg_getSelectedName">FileDlg.getSelectedName</a></li>
<li><a href="#Gui_FileDlg_getSelectedNames">FileDlg.getSelectedNames</a></li>
<li><a href="#Gui_FileDlg_IWnd_onDestroyEvent">IWnd.onDestroyEvent</a></li>
<li><a href="#Gui_Dialog_IWnd_onKeyEvent">IWnd.onKeyEvent</a></li>
<li><a href="#Gui_Dialog_IWnd_onSysCommandEvent">IWnd.onSysCommandEvent</a></li>
<li><a href="#Gui_MessageDlg_confirm">MessageDlg.confirm</a></li>
<li><a href="#Gui_MessageDlg_create">MessageDlg.create</a></li>
<li><a href="#Gui_MessageDlg_doModal">MessageDlg.doModal</a></li>
<li><a href="#Gui_MessageDlg_error">MessageDlg.error</a></li>
<li><a href="#Gui_MessageDlg_ok">MessageDlg.ok</a></li>
<li><a href="#Gui_MessageDlg_okCancel">MessageDlg.okCancel</a></li>
<li><a href="#Gui_MessageDlg_setIcon">MessageDlg.setIcon</a></li>
<li><a href="#Gui_MessageDlg_setIconColor">MessageDlg.setIconColor</a></li>
<li><a href="#Gui_MessageDlg_yesNo">MessageDlg.yesNo</a></li>
<li><a href="#Gui_MessageDlg_yesNoCancel">MessageDlg.yesNoCancel</a></li>
</ul>
<h3>paint</h3>
<ul>
<li><a href="#Gui_BlendColor_apply">BlendColor.apply</a></li>
<li><a href="#Gui_BlendColor_cur">BlendColor.cur</a></li>
<li><a href="#Gui_Cursor_apply">Cursor.apply</a></li>
<li><a href="#Gui_Cursor_clear">Cursor.clear</a></li>
<li><a href="#Gui_Cursor_from">Cursor.from</a></li>
<li><a href="#Gui_Cursor_wait">Cursor.wait</a></li>
<li><a href="#Gui_Icon_clear">Icon.clear</a></li>
<li><a href="#Gui_Icon_from">Icon.from</a></li>
<li><a href="#Gui_Icon_isValid">Icon.isValid</a></li>
<li><a href="#Gui_Icon_opEquals">Icon.opEquals</a></li>
<li><a href="#Gui_Icon_paint">Icon.paint</a></li>
<li><a href="#Gui_Icon_set">Icon.set</a></li>
<li><a href="#Gui_ImageList_countX">ImageList.countX</a></li>
<li><a href="#Gui_ImageList_countY">ImageList.countY</a></li>
<li><a href="#Gui_ImageList_set">ImageList.set</a></li>
<li><a href="#Gui_PaintImage_draw">PaintImage.draw</a></li>
<li><a href="#Gui_PaintImage_init">PaintImage.init</a></li>
<li><a href="#Gui_Theme_createDefaultFont">Theme.createDefaultFont</a></li>
<li><a href="#Gui_Theme_createDefaultFontFS">Theme.createDefaultFontFS</a></li>
<li><a href="#Gui_Theme_drawIconText">Theme.drawIconText</a></li>
<li><a href="#Gui_Theme_drawSubRect">Theme.drawSubRect</a></li>
<li><a href="#Gui_Theme_getIcon24">Theme.getIcon24</a></li>
<li><a href="#Gui_Theme_getIcon64">Theme.getIcon64</a></li>
<li><a href="#Gui_Theme_setTextColors">Theme.setTextColors</a></li>
<li><a href="#Gui_ThemeStyle_addStyleSheetColors">ThemeStyle.addStyleSheetColors</a></li>
<li><a href="#Gui_ThemeStyle_addStyleSheetMetrics">ThemeStyle.addStyleSheetMetrics</a></li>
<li><a href="#Gui_ThemeStyle_clearFont">ThemeStyle.clearFont</a></li>
<li><a href="#Gui_ThemeStyle_clearStyleSheetColors">ThemeStyle.clearStyleSheetColors</a></li>
<li><a href="#Gui_ThemeStyle_create">ThemeStyle.create</a></li>
<li><a href="#Gui_ThemeStyle_getFont">ThemeStyle.getFont</a></li>
<li><a href="#Gui_ThemeStyle_setFont">ThemeStyle.setFont</a></li>
<li><a href="#Gui_ThemeStyle_setStyleSheetColors">ThemeStyle.setStyleSheetColors</a></li>
<li><a href="#Gui_ThemeStyle_setStyleSheetMetrics">ThemeStyle.setStyleSheetMetrics</a></li>
</ul>
<h3>property</h3>
<ul>
<li><a href="#Gui_PropertyList_addItem">PropertyList.addItem</a></li>
<li><a href="#Gui_PropertyList_create">PropertyList.create</a></li>
<li><a href="#Gui_PropertyList_refresh">PropertyList.refresh</a></li>
<li><a href="#Gui_PropertyList_setStringFilter">PropertyList.setStringFilter</a></li>
<li><a href="#Gui_PropertyListCtrl_addItem">PropertyListCtrl.addItem</a></li>
<li><a href="#Gui_PropertyListCtrl_create">PropertyListCtrl.create</a></li>
</ul>
<h3>richedit</h3>
<ul>
<li><a href="#Gui_RichEditLexerSwag_IRichEditLexer_compute">IRichEditLexer.compute</a></li>
<li><a href="#Gui_RichEditLexerSwag_IRichEditLexer_insertRune">IRichEditLexer.insertRune</a></li>
<li><a href="#Gui_RichEditLexerSwag_IRichEditLexer_setup">IRichEditLexer.setup</a></li>
<li><a href="#Gui_RichEditCtrl_addMapping">RichEditCtrl.addMapping</a></li>
<li><a href="#Gui_RichEditCtrl_addText">RichEditCtrl.addText</a></li>
<li><a href="#Gui_RichEditCtrl_canRedo">RichEditCtrl.canRedo</a></li>
<li><a href="#Gui_RichEditCtrl_canUndo">RichEditCtrl.canUndo</a></li>
<li><a href="#Gui_RichEditCtrl_charPressed">RichEditCtrl.charPressed</a></li>
<li><a href="#Gui_RichEditCtrl_checkSelection">RichEditCtrl.checkSelection</a></li>
<li><a href="#Gui_RichEditCtrl_clear">RichEditCtrl.clear</a></li>
<li><a href="#Gui_RichEditCtrl_clearMapping">RichEditCtrl.clearMapping</a></li>
<li><a href="#Gui_RichEditCtrl_clearSelection">RichEditCtrl.clearSelection</a></li>
<li><a href="#Gui_RichEditCtrl_clearUndo">RichEditCtrl.clearUndo</a></li>
<li><a href="#Gui_RichEditCtrl_copyToClipboard">RichEditCtrl.copyToClipboard</a></li>
<li><a href="#Gui_RichEditCtrl_create">RichEditCtrl.create</a></li>
<li><a href="#Gui_RichEditCtrl_deleteBegLine">RichEditCtrl.deleteBegLine</a></li>
<li><a href="#Gui_RichEditCtrl_deleteLeft">RichEditCtrl.deleteLeft</a></li>
<li><a href="#Gui_RichEditCtrl_deleteLine">RichEditCtrl.deleteLine</a></li>
<li><a href="#Gui_RichEditCtrl_deleteRight">RichEditCtrl.deleteRight</a></li>
<li><a href="#Gui_RichEditCtrl_deleteSelection">RichEditCtrl.deleteSelection</a></li>
<li><a href="#Gui_RichEditCtrl_deleteWordLeft">RichEditCtrl.deleteWordLeft</a></li>
<li><a href="#Gui_RichEditCtrl_deleteWordRight">RichEditCtrl.deleteWordRight</a></li>
<li><a href="#Gui_RichEditCtrl_ensureCaretIsVisible">RichEditCtrl.ensureCaretIsVisible</a></li>
<li><a href="#Gui_RichEditCtrl_ensureCursorIsVisible">RichEditCtrl.ensureCursorIsVisible</a></li>
<li><a href="#Gui_RichEditCtrl_evaluateSize">RichEditCtrl.evaluateSize</a></li>
<li><a href="#Gui_RichEditCtrl_executeCommand">RichEditCtrl.executeCommand</a></li>
<li><a href="#Gui_RichEditCtrl_getBegSelection">RichEditCtrl.getBegSelection</a></li>
<li><a href="#Gui_RichEditCtrl_getCaretPos">RichEditCtrl.getCaretPos</a></li>
<li><a href="#Gui_RichEditCtrl_getCursorDisplayPos">RichEditCtrl.getCursorDisplayPos</a></li>
<li><a href="#Gui_RichEditCtrl_getCursorPosFromPoint">RichEditCtrl.getCursorPosFromPoint</a></li>
<li><a href="#Gui_RichEditCtrl_getEndSelection">RichEditCtrl.getEndSelection</a></li>
<li><a href="#Gui_RichEditCtrl_getMappingCommand">RichEditCtrl.getMappingCommand</a></li>
<li><a href="#Gui_RichEditCtrl_getRune">RichEditCtrl.getRune</a></li>
<li><a href="#Gui_RichEditCtrl_getRuneSize">RichEditCtrl.getRuneSize</a></li>
<li><a href="#Gui_RichEditCtrl_getSelectedText">RichEditCtrl.getSelectedText</a></li>
<li><a href="#Gui_RichEditCtrl_getTabOffset">RichEditCtrl.getTabOffset</a></li>
<li><a href="#Gui_RichEditCtrl_getText">RichEditCtrl.getText</a></li>
<li><a href="#Gui_RichEditCtrl_getTextAndStyles">RichEditCtrl.getTextAndStyles</a></li>
<li><a href="#Gui_RichEditCtrl_getXExtent">RichEditCtrl.getXExtent</a></li>
<li><a href="#Gui_RichEditCtrl_insertBegLine">RichEditCtrl.insertBegLine</a></li>
<li><a href="#Gui_RichEditCtrl_insertRune">RichEditCtrl.insertRune</a></li>
<li><a href="#Gui_RichEditCtrl_insertText">RichEditCtrl.insertText</a></li>
<li><a href="#Gui_RichEditCtrl_isReadOnly">RichEditCtrl.isReadOnly</a></li>
<li><a href="#Gui_RichEditCtrl_isSelectionEmpty">RichEditCtrl.isSelectionEmpty</a></li>
<li><a href="#Gui_RichEditCtrl_keyPressed">RichEditCtrl.keyPressed</a></li>
<li><a href="#Gui_RichEditCtrl_lexAll">RichEditCtrl.lexAll</a></li>
<li><a href="#Gui_RichEditCtrl_moveCursorDown">RichEditCtrl.moveCursorDown</a></li>
<li><a href="#Gui_RichEditCtrl_moveCursorEndFile">RichEditCtrl.moveCursorEndFile</a></li>
<li><a href="#Gui_RichEditCtrl_moveCursorEndLine">RichEditCtrl.moveCursorEndLine</a></li>
<li><a href="#Gui_RichEditCtrl_moveCursorLeft">RichEditCtrl.moveCursorLeft</a></li>
<li><a href="#Gui_RichEditCtrl_moveCursorPageDown">RichEditCtrl.moveCursorPageDown</a></li>
<li><a href="#Gui_RichEditCtrl_moveCursorPageUp">RichEditCtrl.moveCursorPageUp</a></li>
<li><a href="#Gui_RichEditCtrl_moveCursorRight">RichEditCtrl.moveCursorRight</a></li>
<li><a href="#Gui_RichEditCtrl_moveCursorStartFile">RichEditCtrl.moveCursorStartFile</a></li>
<li><a href="#Gui_RichEditCtrl_moveCursorStartLine">RichEditCtrl.moveCursorStartLine</a></li>
<li><a href="#Gui_RichEditCtrl_moveCursorUp">RichEditCtrl.moveCursorUp</a></li>
<li><a href="#Gui_RichEditCtrl_moveCursorWordLeft">RichEditCtrl.moveCursorWordLeft</a></li>
<li><a href="#Gui_RichEditCtrl_moveCursorWordRight">RichEditCtrl.moveCursorWordRight</a></li>
<li><a href="#Gui_RichEditCtrl_pasteFromClipboard">RichEditCtrl.pasteFromClipboard</a></li>
<li><a href="#Gui_RichEditCtrl_popUndo">RichEditCtrl.popUndo</a></li>
<li><a href="#Gui_RichEditCtrl_pushUndo">RichEditCtrl.pushUndo</a></li>
<li><a href="#Gui_RichEditCtrl_redo">RichEditCtrl.redo</a></li>
<li><a href="#Gui_RichEditCtrl_scrollLineDown">RichEditCtrl.scrollLineDown</a></li>
<li><a href="#Gui_RichEditCtrl_scrollLineUp">RichEditCtrl.scrollLineUp</a></li>
<li><a href="#Gui_RichEditCtrl_selectAll">RichEditCtrl.selectAll</a></li>
<li><a href="#Gui_RichEditCtrl_selectWord">RichEditCtrl.selectWord</a></li>
<li><a href="#Gui_RichEditCtrl_setCursorPos">RichEditCtrl.setCursorPos</a></li>
<li><a href="#Gui_RichEditCtrl_setDefaultMapping">RichEditCtrl.setDefaultMapping</a></li>
<li><a href="#Gui_RichEditCtrl_setFocus">RichEditCtrl.setFocus</a></li>
<li><a href="#Gui_RichEditCtrl_setForm">RichEditCtrl.setForm</a></li>
<li><a href="#Gui_RichEditCtrl_setLexer">RichEditCtrl.setLexer</a></li>
<li><a href="#Gui_RichEditCtrl_setRectangularSelection">RichEditCtrl.setRectangularSelection</a></li>
<li><a href="#Gui_RichEditCtrl_setRichEdFlags">RichEditCtrl.setRichEdFlags</a></li>
<li><a href="#Gui_RichEditCtrl_setSelection">RichEditCtrl.setSelection</a></li>
<li><a href="#Gui_RichEditCtrl_setText">RichEditCtrl.setText</a></li>
<li><a href="#Gui_RichEditCtrl_undo">RichEditCtrl.undo</a></li>
<li><a href="#Gui_RichEditRuneStyle_opEquals">RichEditRuneStyle.opEquals</a></li>
</ul>
<h3>widgets</h3>
<ul>
<li><a href="#Gui_CheckButton_IWnd_onMouseEvent">CheckButton.IWnd.onMouseEvent</a></li>
<li><a href="#Gui_CheckButton_IWnd_onPaintEvent">CheckButton.IWnd.onPaintEvent</a></li>
<li><a href="#Gui_CheckButton_create">CheckButton.create</a></li>
<li><a href="#Gui_ColorPicker_IWnd_onMouseEvent">ColorPicker.IWnd.onMouseEvent</a></li>
<li><a href="#Gui_ColorPicker_IWnd_onPaintEvent">ColorPicker.IWnd.onPaintEvent</a></li>
<li><a href="#Gui_ColorPicker_change">ColorPicker.change</a></li>
<li><a href="#Gui_ColorPicker_create">ColorPicker.create</a></li>
<li><a href="#Gui_ColorPicker_getColor">ColorPicker.getColor</a></li>
<li><a href="#Gui_ColorPicker_setColor">ColorPicker.setColor</a></li>
<li><a href="#Gui_ColorPicker_setMode">ColorPicker.setMode</a></li>
<li><a href="#Gui_ComboBox_IWnd_onMouseEvent">ComboBox.IWnd.onMouseEvent</a></li>
<li><a href="#Gui_ComboBox_IWnd_onPaintEvent">ComboBox.IWnd.onPaintEvent</a></li>
<li><a href="#Gui_ComboBox_IWnd_onResizeEvent">ComboBox.IWnd.onResizeEvent</a></li>
<li><a href="#Gui_ComboBox_addItem">ComboBox.addItem</a></li>
<li><a href="#Gui_ComboBox_addSeparator">ComboBox.addSeparator</a></li>
<li><a href="#Gui_ComboBox_clear">ComboBox.clear</a></li>
<li><a href="#Gui_ComboBox_create">ComboBox.create</a></li>
<li><a href="#Gui_ComboBox_getSelectedItem">ComboBox.getSelectedItem</a></li>
<li><a href="#Gui_ComboBox_selectItem">ComboBox.selectItem</a></li>
<li><a href="#Gui_EditBox_IWnd_onMouseEvent">EditBox.IWnd.onMouseEvent</a></li>
<li><a href="#Gui_EditBox_IWnd_onPaintEvent">EditBox.IWnd.onPaintEvent</a></li>
<li><a href="#Gui_EditBox_create">EditBox.create</a></li>
<li><a href="#Gui_EditBox_deleteSelection">EditBox.deleteSelection</a></li>
<li><a href="#Gui_EditBox_getText">EditBox.getText</a></li>
<li><a href="#Gui_EditBox_selectAll">EditBox.selectAll</a></li>
<li><a href="#Gui_EditBox_setForm">EditBox.setForm</a></li>
<li><a href="#Gui_EditBox_setInputF64">EditBox.setInputF64</a></li>
<li><a href="#Gui_EditBox_setInputS64">EditBox.setInputS64</a></li>
<li><a href="#Gui_EditBox_setInputU64">EditBox.setInputU64</a></li>
<li><a href="#Gui_EditBox_setNote">EditBox.setNote</a></li>
<li><a href="#Gui_EditBox_setSelection">EditBox.setSelection</a></li>
<li><a href="#Gui_EditBox_setText">EditBox.setText</a></li>
<li><a href="#Gui_EditBox_setTextSilent">EditBox.setTextSilent</a></li>
<li><a href="#Gui_Header_IWnd_onMouseEvent">Header.IWnd.onMouseEvent</a></li>
<li><a href="#Gui_Header_IWnd_onPaintEvent">Header.IWnd.onPaintEvent</a></li>
<li><a href="#Gui_Header_IWnd_paintItem">Header.IWnd.paintItem</a></li>
<li><a href="#Gui_Header_addItem">Header.addItem</a></li>
<li><a href="#Gui_Header_create">Header.create</a></li>
<li><a href="#Gui_Header_getItem">Header.getItem</a></li>
<li><a href="#Gui_Header_getItemByCol">Header.getItemByCol</a></li>
<li><a href="#Gui_Header_getItemPosition">Header.getItemPosition</a></li>
<li><a href="#Gui_Header_getItemPositionByCol">Header.getItemPositionByCol</a></li>
<li><a href="#Gui_Header_getItemWidth">Header.getItemWidth</a></li>
<li><a href="#Gui_Header_getItemWidthByCol">Header.getItemWidthByCol</a></li>
<li><a href="#Gui_Header_getTotalItemsWidth">Header.getTotalItemsWidth</a></li>
<li><a href="#Gui_Header_setItemWidth">Header.setItemWidth</a></li>
<li><a href="#Gui_Header_setItemWidthByCol">Header.setItemWidthByCol</a></li>
<li><a href="#Gui_ColorPicker_IWnd_computeXY">IWnd.computeXY</a></li>
<li><a href="#Gui_IconButton_IWnd_hidePopup">IWnd.hidePopup</a></li>
<li><a href="#Gui_IconButton_IWnd_onApplyStateEvent">IWnd.onApplyStateEvent</a></li>
<li><a href="#Gui_IconBar_IWnd_onComputeStateEvent">IWnd.onComputeStateEvent</a></li>
<li><a href="#Gui_IconButton_IWnd_onDestroyEvent">IWnd.onDestroyEvent</a></li>
<li><a href="#Gui_EditBox_IWnd_onFocusEvent">IWnd.onFocusEvent</a></li>
<li><a href="#Gui_IconButton_IWnd_onHookEvent">IWnd.onHookEvent</a></li>
<li><a href="#Gui_EditBox_IWnd_onKeyEvent">IWnd.onKeyEvent</a></li>
<li><a href="#Gui_ColorPicker_IWnd_onPostPaintEvent">IWnd.onPostPaintEvent</a></li>
<li><a href="#Gui_EditBox_IWnd_onTimerEvent">IWnd.onTimerEvent</a></li>
<li><a href="#Gui_IconBar_addCheckableItem">IconBar.addCheckableItem</a></li>
<li><a href="#Gui_IconBar_addCheckableToggleItem">IconBar.addCheckableToggleItem</a></li>
<li><a href="#Gui_IconBar_addItem">IconBar.addItem</a></li>
<li><a href="#Gui_IconBar_create">IconBar.create</a></li>
<li><a href="#Gui_IconBar_setIconBarFlags">IconBar.setIconBarFlags</a></li>
<li><a href="#Gui_IconButton_IWnd_onMouseEvent">IconButton.IWnd.onMouseEvent</a></li>
<li><a href="#Gui_IconButton_IWnd_onPaintEvent">IconButton.IWnd.onPaintEvent</a></li>
<li><a href="#Gui_IconButton_create">IconButton.create</a></li>
<li><a href="#Gui_IconButton_setCheck">IconButton.setCheck</a></li>
<li><a href="#Gui_IconButton_setForm">IconButton.setForm</a></li>
<li><a href="#Gui_ImageRect_IWnd_onPaintEvent">ImageRect.IWnd.onPaintEvent</a></li>
<li><a href="#Gui_ImageRect_clear">ImageRect.clear</a></li>
<li><a href="#Gui_ImageRect_create">ImageRect.create</a></li>
<li><a href="#Gui_ImageRect_setFile">ImageRect.setFile</a></li>
<li><a href="#Gui_ImageRect_setImage">ImageRect.setImage</a></li>
<li><a href="#Gui_ImageRect_setMovie">ImageRect.setMovie</a></li>
<li><a href="#Gui_Label_IWnd_onPaintEvent">Label.IWnd.onPaintEvent</a></li>
<li><a href="#Gui_Label_create">Label.create</a></li>
<li><a href="#Gui_Label_getText">Label.getText</a></li>
<li><a href="#Gui_Label_setText">Label.setText</a></li>
<li><a href="#Gui_PalettePicker_IWnd_onMouseEvent">PalettePicker.IWnd.onMouseEvent</a></li>
<li><a href="#Gui_PalettePicker_IWnd_onPaintEvent">PalettePicker.IWnd.onPaintEvent</a></li>
<li><a href="#Gui_PalettePicker_addColor">PalettePicker.addColor</a></li>
<li><a href="#Gui_PalettePicker_addHueColors">PalettePicker.addHueColors</a></li>
<li><a href="#Gui_PalettePicker_addLumColors">PalettePicker.addLumColors</a></li>
<li><a href="#Gui_PalettePicker_addSatColors">PalettePicker.addSatColors</a></li>
<li><a href="#Gui_PalettePicker_create">PalettePicker.create</a></li>
<li><a href="#Gui_ProgressBar_IWnd_onPaintEvent">ProgressBar.IWnd.onPaintEvent</a></li>
<li><a href="#Gui_ProgressBar_create">ProgressBar.create</a></li>
<li><a href="#Gui_ProgressBar_setProgression">ProgressBar.setProgression</a></li>
<li><a href="#Gui_PushButton_IWnd_onMouseEvent">PushButton.IWnd.onMouseEvent</a></li>
<li><a href="#Gui_PushButton_IWnd_onPaintEvent">PushButton.IWnd.onPaintEvent</a></li>
<li><a href="#Gui_PushButton_create">PushButton.create</a></li>
<li><a href="#Gui_RadioButton_IWnd_onMouseEvent">RadioButton.IWnd.onMouseEvent</a></li>
<li><a href="#Gui_RadioButton_IWnd_onPaintEvent">RadioButton.IWnd.onPaintEvent</a></li>
<li><a href="#Gui_RadioButton_create">RadioButton.create</a></li>
<li><a href="#Gui_RadioButton_setChecked">RadioButton.setChecked</a></li>
<li><a href="#Gui_Slider_IWnd_onMouseEvent">Slider.IWnd.onMouseEvent</a></li>
<li><a href="#Gui_Slider_IWnd_onPaintEvent">Slider.IWnd.onPaintEvent</a></li>
<li><a href="#Gui_Slider_create">Slider.create</a></li>
<li><a href="#Gui_Slider_getValue">Slider.getValue</a></li>
<li><a href="#Gui_Slider_getValueLeft">Slider.getValueLeft</a></li>
<li><a href="#Gui_Slider_getValueRight">Slider.getValueRight</a></li>
<li><a href="#Gui_Slider_setMinMax">Slider.setMinMax</a></li>
<li><a href="#Gui_Slider_setRangeValues">Slider.setRangeValues</a></li>
<li><a href="#Gui_Slider_setValue">Slider.setValue</a></li>
<li><a href="#Gui_Slider_setValueLeft">Slider.setValueLeft</a></li>
<li><a href="#Gui_Slider_setValueRight">Slider.setValueRight</a></li>
<li><a href="#Gui_Tab_IWnd_onMouseEvent">Tab.IWnd.onMouseEvent</a></li>
<li><a href="#Gui_Tab_IWnd_onPaintEvent">Tab.IWnd.onPaintEvent</a></li>
<li><a href="#Gui_Tab_IWnd_onResizeEvent">Tab.IWnd.onResizeEvent</a></li>
<li><a href="#Gui_Tab_IWnd_paintItem">Tab.IWnd.paintItem</a></li>
<li><a href="#Gui_Tab_addItem">Tab.addItem</a></li>
<li><a href="#Gui_Tab_create">Tab.create</a></li>
<li><a href="#Gui_Tab_select">Tab.select</a></li>
<li><a href="#Gui_Tab_setForm">Tab.setForm</a></li>
<li><a href="#Gui_ToggleButton_IWnd_onMouseEvent">ToggleButton.IWnd.onMouseEvent</a></li>
<li><a href="#Gui_ToggleButton_IWnd_onPaintEvent">ToggleButton.IWnd.onPaintEvent</a></li>
<li><a href="#Gui_ToggleButton_create">ToggleButton.create</a></li>
</ul>
<h3>wnd</h3>
<ul>
<li><a href="#Gui_FrameWnd_IWnd_onPaintEvent">FrameWnd.IWnd.onPaintEvent</a></li>
<li><a href="#Gui_FrameWnd_IWnd_onPostPaintEvent">FrameWnd.IWnd.onPostPaintEvent</a></li>
<li><a href="#Gui_FrameWnd_IWnd_onPrePaintEvent">FrameWnd.IWnd.onPrePaintEvent</a></li>
<li><a href="#Gui_FrameWnd_IWnd_onResizeEvent">FrameWnd.IWnd.onResizeEvent</a></li>
<li><a href="#Gui_FrameWnd_createView">FrameWnd.createView</a></li>
<li><a href="#Gui_FrameWnd_setFrameFlags">FrameWnd.setFrameFlags</a></li>
<li><a href="#Gui_FrameWnd_setFrameForm">FrameWnd.setFrameForm</a></li>
<li><a href="#Gui_FrameWnd_setView">FrameWnd.setView</a></li>
<li><a href="#Gui_Wnd_IWnd_onApplyStateEvent">IWnd.onApplyStateEvent</a></li>
<li><a href="#Gui_Wnd_IWnd_onCommandEvent">IWnd.onCommandEvent</a></li>
<li><a href="#Gui_Wnd_IWnd_onComputeStateEvent">IWnd.onComputeStateEvent</a></li>
<li><a href="#Gui_Wnd_IWnd_onCreateEvent">IWnd.onCreateEvent</a></li>
<li><a href="#Gui_Wnd_IWnd_onDestroyEvent">IWnd.onDestroyEvent</a></li>
<li><a href="#Gui_Wnd_IWnd_onEvent">IWnd.onEvent</a></li>
<li><a href="#Gui_Wnd_IWnd_onFocusEvent">IWnd.onFocusEvent</a></li>
<li><a href="#Gui_Wnd_IWnd_onFrameEvent">IWnd.onFrameEvent</a></li>
<li><a href="#Gui_Wnd_IWnd_onHookEvent">IWnd.onHookEvent</a></li>
<li><a href="#Gui_Wnd_IWnd_onKeyEvent">IWnd.onKeyEvent</a></li>
<li><a href="#Gui_Wnd_IWnd_onNotifyEvent">IWnd.onNotifyEvent</a></li>
<li><a href="#Gui_Wnd_IWnd_onSerializeStateEvent">IWnd.onSerializeStateEvent</a></li>
<li><a href="#Gui_Wnd_IWnd_onSetThemeEvent">IWnd.onSetThemeEvent</a></li>
<li><a href="#Gui_Wnd_IWnd_onStateEvent">IWnd.onStateEvent</a></li>
<li><a href="#Gui_Wnd_IWnd_onSysCommandEvent">IWnd.onSysCommandEvent</a></li>
<li><a href="#Gui_Wnd_IWnd_onSysUserEvent">IWnd.onSysUserEvent</a></li>
<li><a href="#Gui_Wnd_IWnd_onTimerEvent">IWnd.onTimerEvent</a></li>
<li><a href="#Gui_ScrollWnd_IWnd_onMouseEvent">ScrollWnd.IWnd.onMouseEvent</a></li>
<li><a href="#Gui_ScrollWnd_IWnd_onPaintEvent">ScrollWnd.IWnd.onPaintEvent</a></li>
<li><a href="#Gui_ScrollWnd_IWnd_onResizeEvent">ScrollWnd.IWnd.onResizeEvent</a></li>
<li><a href="#Gui_ScrollWnd_getClientScrollRect">ScrollWnd.getClientScrollRect</a></li>
<li><a href="#Gui_ScrollWnd_getScrollPos">ScrollWnd.getScrollPos</a></li>
<li><a href="#Gui_ScrollWnd_setScrollPos">ScrollWnd.setScrollPos</a></li>
<li><a href="#Gui_ScrollWnd_setScrollSize">ScrollWnd.setScrollSize</a></li>
<li><a href="#Gui_Wnd_IWnd_onMouseEvent">Wnd.IWnd.onMouseEvent</a></li>
<li><a href="#Gui_Wnd_IWnd_onPaintEvent">Wnd.IWnd.onPaintEvent</a></li>
<li><a href="#Gui_Wnd_IWnd_onPostPaintEvent">Wnd.IWnd.onPostPaintEvent</a></li>
<li><a href="#Gui_Wnd_IWnd_onPrePaintEvent">Wnd.IWnd.onPrePaintEvent</a></li>
<li><a href="#Gui_Wnd_IWnd_onResizeEvent">Wnd.IWnd.onResizeEvent</a></li>
<li><a href="#Gui_Wnd_applyLayout">Wnd.applyLayout</a></li>
<li><a href="#Gui_Wnd_bringToFront">Wnd.bringToFront</a></li>
<li><a href="#Gui_Wnd_bringToTop">Wnd.bringToTop</a></li>
<li><a href="#Gui_Wnd_captureMouse">Wnd.captureMouse</a></li>
<li><a href="#Gui_Wnd_computeStyle">Wnd.computeStyle</a></li>
<li><a href="#Gui_Wnd_create">Wnd.create</a></li>
<li><a href="#Gui_Wnd_destroy">Wnd.destroy</a></li>
<li><a href="#Gui_Wnd_destroyNow">Wnd.destroyNow</a></li>
<li><a href="#Gui_Wnd_disable">Wnd.disable</a></li>
<li><a href="#Gui_Wnd_enable">Wnd.enable</a></li>
<li><a href="#Gui_Wnd_fitPosInParent">Wnd.fitPosInParent</a></li>
<li><a href="#Gui_Wnd_getApp">Wnd.getApp</a></li>
<li><a href="#Gui_Wnd_getChildById">Wnd.getChildById</a></li>
<li><a href="#Gui_Wnd_getClientRect">Wnd.getClientRect</a></li>
<li><a href="#Gui_Wnd_getClientRectPadding">Wnd.getClientRectPadding</a></li>
<li><a href="#Gui_Wnd_getFocus">Wnd.getFocus</a></li>
<li><a href="#Gui_Wnd_getFont">Wnd.getFont</a></li>
<li><a href="#Gui_Wnd_getKeyShortcut">Wnd.getKeyShortcut</a></li>
<li><a href="#Gui_Wnd_getKeyShortcutNameFor">Wnd.getKeyShortcutNameFor</a></li>
<li><a href="#Gui_Wnd_getMouseCapture">Wnd.getMouseCapture</a></li>
<li><a href="#Gui_Wnd_getOwner">Wnd.getOwner</a></li>
<li><a href="#Gui_Wnd_getParentById">Wnd.getParentById</a></li>
<li><a href="#Gui_Wnd_getRectIn">Wnd.getRectIn</a></li>
<li><a href="#Gui_Wnd_getSurfaceRect">Wnd.getSurfaceRect</a></li>
<li><a href="#Gui_Wnd_getTheme">Wnd.getTheme</a></li>
<li><a href="#Gui_Wnd_getThemeColors">Wnd.getThemeColors</a></li>
<li><a href="#Gui_Wnd_getThemeMetrics">Wnd.getThemeMetrics</a></li>
<li><a href="#Gui_Wnd_getThemeRects">Wnd.getThemeRects</a></li>
<li><a href="#Gui_Wnd_getTiming">Wnd.getTiming</a></li>
<li><a href="#Gui_Wnd_getTopView">Wnd.getTopView</a></li>
<li><a href="#Gui_Wnd_getTopWnd">Wnd.getTopWnd</a></li>
<li><a href="#Gui_Wnd_getWndAt">Wnd.getWndAt</a></li>
<li><a href="#Gui_Wnd_hasFocus">Wnd.hasFocus</a></li>
<li><a href="#Gui_Wnd_hide">Wnd.hide</a></li>
<li><a href="#Gui_Wnd_invalidate">Wnd.invalidate</a></li>
<li><a href="#Gui_Wnd_invalidateRect">Wnd.invalidateRect</a></li>
<li><a href="#Gui_Wnd_isEnabled">Wnd.isEnabled</a></li>
<li><a href="#Gui_Wnd_isParentOf">Wnd.isParentOf</a></li>
<li><a href="#Gui_Wnd_isVisible">Wnd.isVisible</a></li>
<li><a href="#Gui_Wnd_isVisibleState">Wnd.isVisibleState</a></li>
<li><a href="#Gui_Wnd_localToSurface">Wnd.localToSurface</a></li>
<li><a href="#Gui_Wnd_move">Wnd.move</a></li>
<li><a href="#Gui_Wnd_notifyEvent">Wnd.notifyEvent</a></li>
<li><a href="#Gui_Wnd_ownerNotifyEvent">Wnd.ownerNotifyEvent</a></li>
<li><a href="#Gui_Wnd_paint">Wnd.paint</a></li>
<li><a href="#Gui_Wnd_postCommandEvent">Wnd.postCommandEvent</a></li>
<li><a href="#Gui_Wnd_postEvent">Wnd.postEvent</a></li>
<li><a href="#Gui_Wnd_postInvalidateEvent">Wnd.postInvalidateEvent</a></li>
<li><a href="#Gui_Wnd_postQuitEvent">Wnd.postQuitEvent</a></li>
<li><a href="#Gui_Wnd_postResizeEvent">Wnd.postResizeEvent</a></li>
<li><a href="#Gui_Wnd_processEvent">Wnd.processEvent</a></li>
<li><a href="#Gui_Wnd_registerAction">Wnd.registerAction</a></li>
<li><a href="#Gui_Wnd_registerKeyShortcut">Wnd.registerKeyShortcut</a></li>
<li><a href="#Gui_Wnd_releaseMouse">Wnd.releaseMouse</a></li>
<li><a href="#Gui_Wnd_resize">Wnd.resize</a></li>
<li><a href="#Gui_Wnd_screenToSurface">Wnd.screenToSurface</a></li>
<li><a href="#Gui_Wnd_sendCommandEvent">Wnd.sendCommandEvent</a></li>
<li><a href="#Gui_Wnd_sendComputeCommandStateEvent">Wnd.sendComputeCommandStateEvent</a></li>
<li><a href="#Gui_Wnd_sendCreateEvent">Wnd.sendCreateEvent</a></li>
<li><a href="#Gui_Wnd_sendEvent">Wnd.sendEvent</a></li>
<li><a href="#Gui_Wnd_sendResizeEvent">Wnd.sendResizeEvent</a></li>
<li><a href="#Gui_Wnd_sendStateEvent">Wnd.sendStateEvent</a></li>
<li><a href="#Gui_Wnd_serializeState">Wnd.serializeState</a></li>
<li><a href="#Gui_Wnd_setAllMargins">Wnd.setAllMargins</a></li>
<li><a href="#Gui_Wnd_setAllPaddings">Wnd.setAllPaddings</a></li>
<li><a href="#Gui_Wnd_setFocus">Wnd.setFocus</a></li>
<li><a href="#Gui_Wnd_setMargin">Wnd.setMargin</a></li>
<li><a href="#Gui_Wnd_setPadding">Wnd.setPadding</a></li>
<li><a href="#Gui_Wnd_setParent">Wnd.setParent</a></li>
<li><a href="#Gui_Wnd_setPosition">Wnd.setPosition</a></li>
<li><a href="#Gui_Wnd_show">Wnd.show</a></li>
<li><a href="#Gui_Wnd_surfaceToLocal">Wnd.surfaceToLocal</a></li>
<li><a href="#Gui_Wnd_updateCommandState">Wnd.updateCommandState</a></li>
</ul>
</div>
</div>
<div class="right">
<div class="rightpage">
<blockquote>
<b>Work in progress</b>. Generated documentation (swag doc 0.23.0)</blockquote>
<h1>Overview</h1>
<h1>Content</h1>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ActionContext"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">ActionContext</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\action.swg#L1" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>wnd</td>
<td>*Gui.Wnd</td>
<td></td>
</tr>
<tr>
<td>id</td>
<td>Gui.WndId</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_AnchorStyle"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">AnchorStyle</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L18" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>None</td>
<td></td>
</tr>
<tr>
<td>X</td>
<td></td>
</tr>
<tr>
<td>Y</td>
<td></td>
</tr>
<tr>
<td>Width</td>
<td></td>
</tr>
<tr>
<td>Height</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Application"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">Application</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L6" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> native</td>
<td><a href="#Gui_ApplicationNative">Gui.ApplicationNative</a></td>
<td></td>
</tr>
<tr>
<td>sigFrame</td>
<td>Gui.SigArray'(closure(*Gui.Application))</td>
<td></td>
</tr>
<tr>
<td>appIcon</td>
<td>Pixel.Image</td>
<td></td>
</tr>
<tr>
<td>renderer</td>
<td>Pixel.RenderOgl</td>
<td></td>
</tr>
<tr>
<td>theme</td>
<td>*Gui.Theme</td>
<td></td>
</tr>
<tr>
<td>style</td>
<td><a href="#Gui_ThemeStyle">Gui.ThemeStyle</a></td>
<td></td>
</tr>
<tr>
<td>keyb</td>
<td>Core.Input.Keyboard</td>
<td></td>
</tr>
<tr>
<td>mouse</td>
<td>Core.Input.Mouse</td>
<td></td>
</tr>
<tr>
<td>mainSurface</td>
<td>*Gui.Surface</td>
<td></td>
</tr>
<tr>
<td>postedEvents</td>
<td>Core.Array'(*Gui.Event)</td>
<td></td>
</tr>
<tr>
<td>surfaces</td>
<td>Core.Array'(*Gui.Surface)</td>
<td></td>
</tr>
<tr>
<td>postDestroy</td>
<td>Core.Array'(*Gui.Wnd)</td>
<td></td>
</tr>
<tr>
<td>hookEvents</td>
<td>Core.Array'(*Gui.Wnd)</td>
<td></td>
</tr>
<tr>
<td>cursorStack</td>
<td>Core.Array'(Gui.Cursor)</td>
<td></td>
</tr>
<tr>
<td>endModalExit</td>
<td>Gui.WndId</td>
<td></td>
</tr>
<tr>
<td>inModalLoop</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>quitCode</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>timing</td>
<td>Core.Time.FrameTiming</td>
<td></td>
</tr>
<tr>
<td>mustQuit</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>endModal</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>isActivated</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>fadeDisabledSurface</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>mutexPostedEvents</td>
<td>Core.Sync.Mutex</td>
<td></td>
</tr>
<tr>
<td>duringTimerEvents</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>timers</td>
<td>Core.Array'(*Gui.Timer)</td>
<td></td>
</tr>
<tr>
<td>timersToAdd</td>
<td>Core.Array'(*Gui.Timer)</td>
<td></td>
</tr>
<tr>
<td>timersToDelete</td>
<td>Core.Array'(*Gui.Timer)</td>
<td></td>
</tr>
<tr>
<td>frameEvents</td>
<td>Core.Array'(*Gui.Wnd)</td>
<td></td>
</tr>
<tr>
<td>modalSurfaces</td>
<td>Core.Array'(*Gui.Surface)</td>
<td></td>
</tr>
<tr>
<td>toDelete</td>
<td>Core.Array'({ptr: *void, type: const *Swag.TypeInfo})</td>
<td></td>
</tr>
<tr>
<td>mouseEnterWnd</td>
<td>*Gui.Wnd</td>
<td></td>
</tr>
<tr>
<td>mouseCaptureWnd</td>
<td>*Gui.Wnd</td>
<td></td>
</tr>
<tr>
<td>keybFocusWnd</td>
<td>*Gui.Wnd</td>
<td></td>
</tr>
<tr>
<td>maxRunFrame</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>configPath</td>
<td>Core.String</td>
<td></td>
</tr>
<tr>
<td>hotKeys</td>
<td>Core.Array'(Gui.KeyShortcut)</td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_Application_addTimer">addTimer(self, const ref Duration, *Wnd)</a></td>
<td>Register a new timer for the given <code class="incode">target</code>. </td>
</tr>
<tr>
<td><a href="#Gui_Application_createSurface">createSurface(self, s32, s32, s32, s32, SurfaceFlags, *Wnd, HookEvent)</a></td>
<td>Creates a new surface. </td>
</tr>
<tr>
<td><a href="#Gui_Application_deleteTimer">deleteTimer(self, *Timer)</a></td>
<td>Delete and unregister timer. </td>
</tr>
<tr>
<td><a href="#Gui_Application_destroyWnd">destroyWnd(self, *Wnd)</a></td>
<td>Destroy a window  To cleanly close a window, you should call wnd.destroy(). </td>
</tr>
<tr>
<td><a href="#Gui_Application_doModalLoop">doModalLoop(self, *Surface, func(*void, *Application))</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Application_exitModal">exitModal(self, string)</a></td>
<td>End current modal loop. </td>
</tr>
<tr>
<td><a href="#Gui_Application_getDirectoryIcon">getDirectoryIcon(self, string, bool)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Application_getDt">getDt(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Application_getFileIcon">getFileIcon(self, string, bool)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Application_getHotKeyShortcut">getHotKeyShortcut(self, KeyModifiers, Key)</a></td>
<td>Get the id associated with a shortcut. null if none. </td>
</tr>
<tr>
<td><a href="#Gui_Application_getHotKeyShortcutNameFor">getHotKeyShortcutNameFor(self, WndId)</a></td>
<td>Get the name associated with a given id shortcut. </td>
</tr>
<tr>
<td><a href="#Gui_Application_getKeyboard">getKeyboard(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Application_getMouse">getMouse(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Application_getMouseCapture">getMouseCapture(self)</a></td>
<td>Return the captured wnd for mouse. </td>
</tr>
<tr>
<td><a href="#Gui_Application_getRenderer">getRenderer(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Application_getWndAtMouse">getWndAtMouse(self)</a></td>
<td>Returns the window under the given point. </td>
</tr>
<tr>
<td><a href="#Gui_Application_invalidate">invalidate(self)</a></td>
<td>Force all surfaces to be painted. </td>
</tr>
<tr>
<td><a href="#Gui_Application_loadState">loadState(self)</a></td>
<td>Load the application state. </td>
</tr>
<tr>
<td><a href="#Gui_Application_popCursor">popCursor(self)</a></td>
<td>Pop a new mouse cursor. </td>
</tr>
<tr>
<td><a href="#Gui_Application_postEvent">postEvent(self, *Event)</a></td>
<td>Post a new event (thread safe). </td>
</tr>
<tr>
<td><a href="#Gui_Application_postQuitEvent">postQuitEvent(self, s32)</a></td>
<td>Ask to exit the application. </td>
</tr>
<tr>
<td><a href="#Gui_Application_pushCursor">pushCursor(self, const ref Cursor)</a></td>
<td>Push a new mouse cursor. </td>
</tr>
<tr>
<td><a href="#Gui_Application_registerFrameEvent">registerFrameEvent(self, *Wnd)</a></td>
<td>Register a window to receive a FrameEvent each running loop. </td>
</tr>
<tr>
<td><a href="#Gui_Application_registerHookEvents">registerHookEvents(self, *Wnd)</a></td>
<td>Register a window to receive hook events. </td>
</tr>
<tr>
<td><a href="#Gui_Application_registerHotKey">registerHotKey(self, KeyModifiers, Key, WndId, *Wnd)</a></td>
<td>Register a global os key action. </td>
</tr>
<tr>
<td><a href="#Gui_Application_run">run(self)</a></td>
<td>Run until exit. </td>
</tr>
<tr>
<td><a href="#Gui_Application_runFrame">runFrame(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Application_runSurface">runSurface(s32, s32, s32, s32, string, *Wnd, bool, bool, HookEvent, func(*Application))</a></td>
<td>Create a main surface, and run. </td>
</tr>
<tr>
<td><a href="#Gui_Application_saveState">saveState(self)</a></td>
<td>Save the application state. </td>
</tr>
<tr>
<td><a href="#Gui_Application_sendEvent">sendEvent(self, *Event)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Application_setActivated">setActivated(self, bool)</a></td>
<td>Set activated state. </td>
</tr>
<tr>
<td><a href="#Gui_Application_setAppIcon">setAppIcon(self, const ref Image)</a></td>
<td>Associate a configuration file. </td>
</tr>
<tr>
<td><a href="#Gui_Application_setConfigPath">setConfigPath(self, string)</a></td>
<td>Associate a configuration file. </td>
</tr>
<tr>
<td><a href="#Gui_Application_unregisterFrameEvent">unregisterFrameEvent(self, *Wnd)</a></td>
<td>Unregister a window that receives a FrameEvent each running loop. </td>
</tr>
<tr>
<td><a href="#Gui_Application_unregisterHookEvents">unregisterHookEvents(self, *Wnd)</a></td>
<td>Unregister a window to receive hook events. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Application_addTimer"><span class="titletype">func</span> <span class="titlelight">Application.</span><span class="titlestrong">addTimer</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L711" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Register a new timer for the given <code class="incode">target</code>. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">addTimer</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, time: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Time</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Duration</span><span class="SyntaxCode">, target: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Timer">Timer</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Application_createSurface"><span class="titletype">func</span> <span class="titlelight">Application.</span><span class="titlestrong">createSurface</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L542" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Creates a new surface. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">createSurface</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, width: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, height: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, flags = </span><span class="SyntaxConstant"><a href="#Gui_SurfaceFlags">SurfaceFlags</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant">OverlappedWindow</span><span class="SyntaxCode">, view: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">, hook: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HookEvent</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Surface">Surface</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Application_deleteTimer"><span class="titletype">func</span> <span class="titlelight">Application.</span><span class="titlestrong">deleteTimer</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L724" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Delete and unregister timer. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">deleteTimer</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, timer: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Timer">Timer</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Application_destroyWnd"><span class="titletype">func</span> <span class="titlelight">Application.</span><span class="titlestrong">destroyWnd</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L638" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Destroy a window  To cleanly close a window, you should call wnd.destroy(). </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">destroyWnd</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, wnd: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Application_doModalLoop"><span class="titletype">func</span> <span class="titlelight">Application.</span><span class="titlestrong">doModalLoop</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L451" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">doModalLoop</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, surface: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Surface">Surface</a></span><span class="SyntaxCode">, cb: </span><span class="SyntaxKeyword">closure</span><span class="SyntaxCode">(self) = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Application_exitModal"><span class="titletype">func</span> <span class="titlelight">Application.</span><span class="titlestrong">exitModal</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L806" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>End current modal loop. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">exitModal</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, exitId: </span><span class="SyntaxType">string</span><span class="SyntaxCode"> = </span><span class="SyntaxString">""</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Application_getDirectoryIcon"><span class="titletype">func</span> <span class="titlelight">Application.</span><span class="titlestrong">getDirectoryIcon</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L813" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getDirectoryIcon</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, name: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, small = </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Application_getDt"><span class="titletype">func</span> <span class="titlelight">Application.</span><span class="titlestrong">getDt</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L523" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getDt</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Application_getFileIcon"><span class="titletype">func</span> <span class="titlelight">Application.</span><span class="titlestrong">getFileIcon</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L812" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getFileIcon</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, name: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, small = </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Application_getHotKeyShortcut"><span class="titletype">func</span> <span class="titlelight">Application.</span><span class="titlestrong">getHotKeyShortcut</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L833" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the id associated with a shortcut. null if none. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getHotKeyShortcut</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, mdf: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Input</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">KeyModifiers</span><span class="SyntaxCode">, key: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Input</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Key</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode">
{ 
	id: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode">
	wnd: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode">
}</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Application_getHotKeyShortcutNameFor"><span class="titletype">func</span> <span class="titlelight">Application.</span><span class="titlestrong">getHotKeyShortcutNameFor</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L845" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the name associated with a given id shortcut. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getHotKeyShortcutNameFor</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, wndId: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">String</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Application_getKeyboard"><span class="titletype">func</span> <span class="titlelight">Application.</span><span class="titlestrong">getKeyboard</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L521" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getKeyboard</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Application_getMouse"><span class="titletype">func</span> <span class="titlelight">Application.</span><span class="titlestrong">getMouse</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L522" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getMouse</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Application_getMouseCapture"><span class="titletype">func</span> <span class="titlelight">Application.</span><span class="titlestrong">getMouseCapture</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L631" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Return the captured wnd for mouse. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getMouseCapture</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Application_getRenderer"><span class="titletype">func</span> <span class="titlelight">Application.</span><span class="titlestrong">getRenderer</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L520" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getRenderer</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Application_getWndAtMouse"><span class="titletype">func</span> <span class="titlelight">Application.</span><span class="titlestrong">getWndAtMouse</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L661" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the window under the given point. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getWndAtMouse</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Application_invalidate"><span class="titletype">func</span> <span class="titlelight">Application.</span><span class="titlestrong">invalidate</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L684" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Force all surfaces to be painted. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">invalidate</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Application_loadState"><span class="titletype">func</span> <span class="titlelight">Application.</span><span class="titlestrong">loadState</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L616" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Load the application state. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">loadState</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Application_popCursor"><span class="titletype">func</span> <span class="titlelight">Application.</span><span class="titlestrong">popCursor</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L773" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Pop a new mouse cursor. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">popCursor</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Application_postEvent"><span class="titletype">func</span> <span class="titlelight">Application.</span><span class="titlestrong">postEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L699" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Post a new event (thread safe). </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">postEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, event: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Event">Event</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Application_postQuitEvent"><span class="titletype">func</span> <span class="titlelight">Application.</span><span class="titlestrong">postQuitEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L691" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Ask to exit the application. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">postQuitEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, quitCode: </span><span class="SyntaxType">s32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Application_pushCursor"><span class="titletype">func</span> <span class="titlelight">Application.</span><span class="titlestrong">pushCursor</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L767" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Push a new mouse cursor. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">pushCursor</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, cursor: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Cursor">Cursor</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Application_registerFrameEvent"><span class="titletype">func</span> <span class="titlelight">Application.</span><span class="titlestrong">registerFrameEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L741" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Register a window to receive a FrameEvent each running loop. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">registerFrameEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, target: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Application_registerHookEvents"><span class="titletype">func</span> <span class="titlelight">Application.</span><span class="titlestrong">registerHookEvents</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L754" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Register a window to receive hook events. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">registerHookEvents</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, target: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Application_registerHotKey"><span class="titletype">func</span> <span class="titlelight">Application.</span><span class="titlestrong">registerHotKey</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L816" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Register a global os key action. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">registerHotKey</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, mdf: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Input</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">KeyModifiers</span><span class="SyntaxCode">, key: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Input</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Key</span><span class="SyntaxCode">, wndId: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode">, target: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Application_run"><span class="titletype">func</span> <span class="titlelight">Application.</span><span class="titlestrong">run</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L790" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Run until exit. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">run</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Application_runFrame"><span class="titletype">func</span> <span class="titlelight">Application.</span><span class="titlestrong">runFrame</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L425" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">runFrame</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Application_runSurface"><span class="titletype">func</span> <span class="titlelight">Application.</span><span class="titlestrong">runSurface</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L527" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Create a main surface, and run. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">runSurface</span><span class="SyntaxCode">(x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, width: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, height: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, title: </span><span class="SyntaxType">string</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">, view: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">, borders = </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">, canSize = </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">, hook: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HookEvent</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">, init: </span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Application">Application</a></span><span class="SyntaxCode">) = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Application_saveState"><span class="titletype">func</span> <span class="titlelight">Application.</span><span class="titlestrong">saveState</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L600" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Save the application state. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">saveState</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Application_sendEvent"><span class="titletype">func</span> <span class="titlelight">Application.</span><span class="titlestrong">sendEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L705" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">sendEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, event: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Event">Event</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Application_setActivated"><span class="titletype">func</span> <span class="titlelight">Application.</span><span class="titlestrong">setActivated</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L673" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set activated state. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setActivated</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, activated: </span><span class="SyntaxType">bool</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Application_setAppIcon"><span class="titletype">func</span> <span class="titlelight">Application.</span><span class="titlestrong">setAppIcon</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L586" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Associate a configuration file. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setAppIcon</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, img: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Image</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Application_setConfigPath"><span class="titletype">func</span> <span class="titlelight">Application.</span><span class="titlestrong">setConfigPath</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L580" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Associate a configuration file. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setConfigPath</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, path: </span><span class="SyntaxType">string</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Application_unregisterFrameEvent"><span class="titletype">func</span> <span class="titlelight">Application.</span><span class="titlestrong">unregisterFrameEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L748" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Unregister a window that receives a FrameEvent each running loop. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">unregisterFrameEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, target: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Application_unregisterHookEvents"><span class="titletype">func</span> <span class="titlelight">Application.</span><span class="titlestrong">unregisterHookEvents</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L761" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Unregister a window to receive hook events. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">unregisterHookEvents</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, target: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ApplicationNative"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">ApplicationNative</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.win32.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>fileSmallIconsImage</td>
<td>Pixel.Image</td>
<td></td>
</tr>
<tr>
<td>fileSmallIconsImageList</td>
<td><a href="#Gui_ImageList">Gui.ImageList</a></td>
<td></td>
</tr>
<tr>
<td>mapFileSmallIcons</td>
<td>Core.HashTable'(string, s32)</td>
<td></td>
</tr>
<tr>
<td>fileBigIconsImage</td>
<td>Pixel.Image</td>
<td></td>
</tr>
<tr>
<td>fileBigIconsImageList</td>
<td><a href="#Gui_ImageList">Gui.ImageList</a></td>
<td></td>
</tr>
<tr>
<td>mapFileBigIcons</td>
<td>Core.HashTable'(string, s32)</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_BackgroundStyle"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">BackgroundStyle</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L27" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>Transparent</td>
<td></td>
</tr>
<tr>
<td>Window</td>
<td></td>
</tr>
<tr>
<td>Dialog</td>
<td></td>
</tr>
<tr>
<td>DialogBar</td>
<td></td>
</tr>
<tr>
<td>View</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_BlendColor"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">BlendColor</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\blendcolor.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>curVec4</td>
<td>Core.Math.Vector4</td>
<td></td>
</tr>
<tr>
<td>startVec4</td>
<td>Core.Math.Vector4</td>
<td></td>
</tr>
<tr>
<td>destVec4</td>
<td>Core.Math.Vector4</td>
<td></td>
</tr>
<tr>
<td>factor</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>start</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>speed</td>
<td>f32</td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_BlendColor_apply">apply(self, *Wnd, const ref Color)</a></td>
<td>Apply the color lerp to reach <code class="incode">target</code>. </td>
</tr>
<tr>
<td><a href="#Gui_BlendColor_cur">cur(self)</a></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_BlendColor_apply"><span class="titletype">func</span> <span class="titlelight">BlendColor.</span><span class="titlestrong">apply</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\blendcolor.swg#L17" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Apply the color lerp to reach <code class="incode">target</code>. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">apply</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, wnd: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode">, target: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Color</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_BlendColor_cur"><span class="titletype">func</span> <span class="titlelight">BlendColor.</span><span class="titlestrong">cur</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\blendcolor.swg#L60" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">cur</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Color</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Button"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">Button</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\button.swg#L1" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> wnd</td>
<td><a href="#Gui_Wnd">Gui.Wnd</a></td>
<td></td>
</tr>
<tr>
<td>icon</td>
<td><a href="#Gui_Icon">Gui.Icon</a></td>
<td></td>
</tr>
<tr>
<td>sigPressed</td>
<td>Gui.SigArray'(closure(*Gui.Button))</td>
<td></td>
</tr>
<tr>
<td>sigRightPressed</td>
<td>Gui.SigArray'(closure(*Gui.Button))</td>
<td></td>
</tr>
<tr>
<td>isPressed</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>isPressing</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>isHot</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>isIn</td>
<td>bool</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ButtonCheckState"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">ButtonCheckState</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\checkbutton.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>UnChecked</td>
<td></td>
</tr>
<tr>
<td>Checked</td>
<td></td>
</tr>
<tr>
<td>Undefined</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_CheckButton"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">CheckButton</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\checkbutton.swg#L19" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> button</td>
<td><a href="#Gui_Button">Gui.Button</a></td>
<td></td>
</tr>
<tr>
<td>checked</td>
<td><a href="#Gui_ButtonCheckState">Gui.ButtonCheckState</a></td>
<td></td>
</tr>
<tr>
<td>checkButtonFlags</td>
<td><a href="#Gui_CheckButtonFlags">Gui.CheckButtonFlags</a></td>
<td></td>
</tr>
<tr>
<td>sigChanged</td>
<td>Gui.SigArray'(closure(*Gui.CheckButton))</td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_CheckButton_create">create(*Wnd, string, const ref Rectangle, WndId)</a></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_CheckButton_IWnd_onMouseEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onMouseEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\checkbutton.swg#L120" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onMouseEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_MouseEvent">MouseEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_CheckButton_IWnd_onPaintEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onPaintEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\checkbutton.swg#L30" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onPaintEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_PaintEvent">PaintEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_CheckButton_create"><span class="titletype">func</span> <span class="titlelight">CheckButton.</span><span class="titlestrong">create</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\checkbutton.swg#L139" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">create</span><span class="SyntaxCode">(parent: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode">, name: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, position: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode"> = {}, id: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_CheckButton">CheckButton</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_CheckButtonFlags"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">CheckButtonFlags</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\checkbutton.swg#L12" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>Zero</td>
<td></td>
</tr>
<tr>
<td>ThreeState</td>
<td></td>
</tr>
<tr>
<td>RightAlign</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Clipboard_addData"><span class="titletype">func</span> <span class="titlelight">Clipboard.</span><span class="titlestrong">addData</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\clipboard.win32.swg#L27" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add data of a given format to the clipboard. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">addData</span><span class="SyntaxCode">(fmt: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Clipboard</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Format</span><span class="SyntaxCode">, data: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> [..] </span><span class="SyntaxType">u8</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Clipboard_addImage"><span class="titletype">func</span> <span class="titlelight">Clipboard.</span><span class="titlestrong">addImage</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\clipboard.win32.swg#L85" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add an image. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">addImage</span><span class="SyntaxCode">(image: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Image</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Clipboard_addString"><span class="titletype">func</span> <span class="titlelight">Clipboard.</span><span class="titlestrong">addString</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\clipboard.win32.swg#L200" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add string to clipboard. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">addString</span><span class="SyntaxCode">(str: </span><span class="SyntaxType">string</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Clipboard_getData"><span class="titletype">func</span> <span class="titlelight">Clipboard.</span><span class="titlestrong">getData</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\clipboard.win32.swg#L44" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get data of the given format. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getData</span><span class="SyntaxCode">(fmt: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Clipboard</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Format</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Array</span><span class="SyntaxCode">'(</span><span class="SyntaxType">u8</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Clipboard_getImage"><span class="titletype">func</span> <span class="titlelight">Clipboard.</span><span class="titlestrong">getImage</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\clipboard.win32.swg#L142" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get image from clipboard. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getImage</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Image</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Clipboard_getString"><span class="titletype">func</span> <span class="titlelight">Clipboard.</span><span class="titlestrong">getString</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\clipboard.win32.swg#L229" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get utf8 string from clipboard. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getString</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">String</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Clipboard_hasFormat"><span class="titletype">func</span> <span class="titlelight">Clipboard.</span><span class="titlestrong">hasFormat</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\clipboard.win32.swg#L17" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns true if the clipboard contains the given format. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">hasFormat</span><span class="SyntaxCode">(fmt: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Clipboard</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Format</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">bool</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Clipboard_hasImage"><span class="titletype">func</span> <span class="titlelight">Clipboard.</span><span class="titlestrong">hasImage</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\clipboard.win32.swg#L70" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns true if the clipbboard contains an image. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">hasImage</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxType">bool</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Clipboard_hasString"><span class="titletype">func</span> <span class="titlelight">Clipboard.</span><span class="titlestrong">hasString</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\clipboard.win32.swg#L185" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns true if the clipboard contains a string. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">hasString</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxType">bool</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Clipboard_registerFormat"><span class="titletype">func</span> <span class="titlelight">Clipboard.</span><span class="titlestrong">registerFormat</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\clipboard.win32.swg#L11" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Register a new format. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">registerFormat</span><span class="SyntaxCode">(name: </span><span class="SyntaxType">string</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Clipboard</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Format</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ColorPicker"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">ColorPicker</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\colorpicker.swg#L28" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> wnd</td>
<td><a href="#Gui_Wnd">Gui.Wnd</a></td>
<td></td>
</tr>
<tr>
<td>hue</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>sat</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>lum</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>a</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>r</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>g</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>b</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>mode</td>
<td><a href="#Gui_ColorPickerMode">Gui.ColorPickerMode</a></td>
<td></td>
</tr>
<tr>
<td>sigChanged</td>
<td>Gui.SigArray'(closure(*Gui.ColorPicker))</td>
<td></td>
</tr>
<tr>
<td>dirty</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>isMoving</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>img</td>
<td>Pixel.Image</td>
<td></td>
</tr>
<tr>
<td>texture</td>
<td>Pixel.Texture</td>
<td></td>
</tr>
<tr>
<td>xCur</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>yCur</td>
<td>s32</td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_ColorPicker_change">change(self, f32, f32, f32, f32)</a></td>
<td>Change one component, HSL or alpha. </td>
</tr>
<tr>
<td><a href="#Gui_ColorPicker_create">create(*Wnd, const ref Rectangle, WndId)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_ColorPicker_getColor">getColor(self)</a></td>
<td>Returns the selected color. </td>
</tr>
<tr>
<td><a href="#Gui_ColorPicker_setColor">setColor(self, const ref Color)</a></td>
<td>Set the base color, and repaint. </td>
</tr>
<tr>
<td><a href="#Gui_ColorPicker_setMode">setMode(self, ColorPickerMode)</a></td>
<td>Set the color picker box mode. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ColorPicker_IWnd_computeXY"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">computeXY</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\colorpicker.swg#L45" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">computeXY</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ColorPicker_IWnd_onMouseEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onMouseEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\colorpicker.swg#L277" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onMouseEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_MouseEvent">MouseEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ColorPicker_IWnd_onPaintEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onPaintEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\colorpicker.swg#L118" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onPaintEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_PaintEvent">PaintEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ColorPicker_IWnd_onPostPaintEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onPostPaintEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\colorpicker.swg#L247" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onPostPaintEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_PaintEvent">PaintEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ColorPicker_change"><span class="titletype">func</span> <span class="titlelight">ColorPicker.</span><span class="titlestrong">change</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\colorpicker.swg#L360" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Change one component, HSL or alpha. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">change</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, h: </span><span class="SyntaxType">f32</span><span class="SyntaxCode"> = -</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">, s: </span><span class="SyntaxType">f32</span><span class="SyntaxCode"> = -</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">, l: </span><span class="SyntaxType">f32</span><span class="SyntaxCode"> = -</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">, a: </span><span class="SyntaxType">f32</span><span class="SyntaxCode"> = -</span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ColorPicker_create"><span class="titletype">func</span> <span class="titlelight">ColorPicker.</span><span class="titlestrong">create</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\colorpicker.swg#L349" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">create</span><span class="SyntaxCode">(parent: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode">, position: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode"> = {}, id: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_ColorPicker">ColorPicker</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ColorPicker_getColor"><span class="titletype">func</span> <span class="titlelight">ColorPicker.</span><span class="titlestrong">getColor</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\colorpicker.swg#L406" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the selected color. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getColor</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Color</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ColorPicker_setColor"><span class="titletype">func</span> <span class="titlelight">ColorPicker.</span><span class="titlestrong">setColor</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\colorpicker.swg#L396" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the base color, and repaint. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setColor</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, col: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Color</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ColorPicker_setMode"><span class="titletype">func</span> <span class="titlelight">ColorPicker.</span><span class="titlestrong">setMode</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\colorpicker.swg#L412" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the color picker box mode. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setMode</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, mode: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_ColorPickerMode">ColorPickerMode</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ColorPickerCtrl"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">ColorPickerCtrl</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\colorpickerctrl.swg#L14" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> frameWnd</td>
<td><a href="#Gui_FrameWnd">Gui.FrameWnd</a></td>
<td></td>
</tr>
<tr>
<td>sigChanged</td>
<td>Gui.SigArray'(closure(*Gui.ColorPickerCtrl))</td>
<td></td>
</tr>
<tr>
<td>oldColor</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>color</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>oldColorDone</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>mode</td>
<td><a href="#Gui_ColorPickerViewMode">Gui.ColorPickerViewMode</a></td>
<td></td>
</tr>
<tr>
<td>staticRes</td>
<td>*Gui.Wnd</td>
<td></td>
</tr>
<tr>
<td>pickBox</td>
<td>*Gui.ColorPicker</td>
<td></td>
</tr>
<tr>
<td>pickBar</td>
<td>*Gui.ColorPicker</td>
<td></td>
</tr>
<tr>
<td>pickAlpha</td>
<td>*Gui.ColorPicker</td>
<td></td>
</tr>
<tr>
<td>editR</td>
<td>*Gui.EditBox</td>
<td></td>
</tr>
<tr>
<td>editG</td>
<td>*Gui.EditBox</td>
<td></td>
</tr>
<tr>
<td>editB</td>
<td>*Gui.EditBox</td>
<td></td>
</tr>
<tr>
<td>editA</td>
<td>*Gui.EditBox</td>
<td></td>
</tr>
<tr>
<td>editH</td>
<td>*Gui.EditBox</td>
<td></td>
</tr>
<tr>
<td>radioH</td>
<td>*Gui.RadioButton</td>
<td></td>
</tr>
<tr>
<td>radioL</td>
<td>*Gui.RadioButton</td>
<td></td>
</tr>
<tr>
<td>radioS</td>
<td>*Gui.RadioButton</td>
<td></td>
</tr>
<tr>
<td>radioR</td>
<td>*Gui.RadioButton</td>
<td></td>
</tr>
<tr>
<td>radioG</td>
<td>*Gui.RadioButton</td>
<td></td>
</tr>
<tr>
<td>radioB</td>
<td>*Gui.RadioButton</td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_ColorPickerCtrl_create">create(*Wnd, const ref Rectangle, WndId)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_ColorPickerCtrl_getColor">getColor(self)</a></td>
<td>Returns the selected color. </td>
</tr>
<tr>
<td><a href="#Gui_ColorPickerCtrl_setColor">setColor(self, const ref Color)</a></td>
<td>Set the selected color. </td>
</tr>
<tr>
<td><a href="#Gui_ColorPickerCtrl_setMode">setMode(self, ColorPickerViewMode)</a></td>
<td>Change the display mode. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ColorPickerCtrl_IWnd_onPaintEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onPaintEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\colorpickerctrl.swg#L41" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onPaintEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_PaintEvent">PaintEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ColorPickerCtrl_create"><span class="titletype">func</span> <span class="titlelight">ColorPickerCtrl.</span><span class="titlestrong">create</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\colorpickerctrl.swg#L116" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">create</span><span class="SyntaxCode">(parent: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode">, position: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode">, id: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_ColorPickerCtrl">ColorPickerCtrl</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ColorPickerCtrl_getColor"><span class="titletype">func</span> <span class="titlelight">ColorPickerCtrl.</span><span class="titlestrong">getColor</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\colorpickerctrl.swg#L354" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the selected color. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getColor</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Color</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ColorPickerCtrl_setColor"><span class="titletype">func</span> <span class="titlelight">ColorPickerCtrl.</span><span class="titlestrong">setColor</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\colorpickerctrl.swg#L360" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the selected color. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setColor</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, color: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Color</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ColorPickerCtrl_setMode"><span class="titletype">func</span> <span class="titlelight">ColorPickerCtrl.</span><span class="titlestrong">setMode</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\colorpickerctrl.swg#L326" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Change the display mode. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setMode</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, mode: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_ColorPickerViewMode">ColorPickerViewMode</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ColorPickerMode"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">ColorPickerMode</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\colorpicker.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>SaturationLightness</td>
<td></td>
</tr>
<tr>
<td>HueSaturation</td>
<td></td>
</tr>
<tr>
<td>HueLightness</td>
<td></td>
</tr>
<tr>
<td>GreenBlue</td>
<td></td>
</tr>
<tr>
<td>RedBlue</td>
<td></td>
</tr>
<tr>
<td>RedGreen</td>
<td></td>
</tr>
<tr>
<td>HueVert</td>
<td></td>
</tr>
<tr>
<td>HueHorz</td>
<td></td>
</tr>
<tr>
<td>LightnessVert</td>
<td></td>
</tr>
<tr>
<td>LightnessHorz</td>
<td></td>
</tr>
<tr>
<td>SaturationVert</td>
<td></td>
</tr>
<tr>
<td>SaturationHorz</td>
<td></td>
</tr>
<tr>
<td>AlphaHorz</td>
<td></td>
</tr>
<tr>
<td>AlphaVert</td>
<td></td>
</tr>
<tr>
<td>RedHorz</td>
<td></td>
</tr>
<tr>
<td>RedVert</td>
<td></td>
</tr>
<tr>
<td>GreenHorz</td>
<td></td>
</tr>
<tr>
<td>GreenVert</td>
<td></td>
</tr>
<tr>
<td>BlueHorz</td>
<td></td>
</tr>
<tr>
<td>BlueVert</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ColorPickerViewMode"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">ColorPickerViewMode</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\colorpickerctrl.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>SaturationLightness</td>
<td></td>
</tr>
<tr>
<td>HueSaturation</td>
<td></td>
</tr>
<tr>
<td>HueLightness</td>
<td></td>
</tr>
<tr>
<td>GreenBlue</td>
<td></td>
</tr>
<tr>
<td>RedBlue</td>
<td></td>
</tr>
<tr>
<td>RedGreen</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ComboBox"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">ComboBox</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\combobox.swg#L17" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> wnd</td>
<td><a href="#Gui_Wnd">Gui.Wnd</a></td>
<td></td>
</tr>
<tr>
<td>kind</td>
<td><a href="#Gui_ComboBoxKind">Gui.ComboBoxKind</a></td>
<td></td>
</tr>
<tr>
<td>minWidthPopup</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>maxHeightPopup</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>sigChanged</td>
<td>Gui.SigArray'(closure(*Gui.ComboBox, u32))</td>
<td></td>
</tr>
<tr>
<td>editBox</td>
<td>*Gui.EditBox</td>
<td></td>
</tr>
<tr>
<td>isHot</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>popup</td>
<td>*Gui.PopupListCtrl</td>
<td></td>
</tr>
<tr>
<td>selectedIdx</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>maxIconSize</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>aniBk</td>
<td><a href="#Gui_BlendColor">Gui.BlendColor</a></td>
<td></td>
</tr>
<tr>
<td>aniBorder</td>
<td><a href="#Gui_BlendColor">Gui.BlendColor</a></td>
<td></td>
</tr>
<tr>
<td>aniText</td>
<td><a href="#Gui_BlendColor">Gui.BlendColor</a></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_ComboBox_addItem">addItem(self, string, WndId, const ref Icon, *void, *void)</a></td>
<td>Add a new item. </td>
</tr>
<tr>
<td><a href="#Gui_ComboBox_addSeparator">addSeparator(self)</a></td>
<td>Add a separator. </td>
</tr>
<tr>
<td><a href="#Gui_ComboBox_clear">clear(self)</a></td>
<td>Remove all items. </td>
</tr>
<tr>
<td><a href="#Gui_ComboBox_create">create(*Wnd, const ref Rectangle, WndId, ComboBoxKind)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_ComboBox_getSelectedItem">getSelectedItem(self)</a></td>
<td>Returns the selected item. </td>
</tr>
<tr>
<td><a href="#Gui_ComboBox_selectItem">selectItem(self, u32)</a></td>
<td>Set the selected item index. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ComboBox_IWnd_onMouseEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onMouseEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\combobox.swg#L125" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onMouseEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_MouseEvent">MouseEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ComboBox_IWnd_onPaintEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onPaintEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\combobox.swg#L52" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onPaintEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_PaintEvent">PaintEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ComboBox_IWnd_onResizeEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onResizeEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\combobox.swg#L40" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onResizeEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_ResizeEvent">ResizeEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ComboBox_addItem"><span class="titletype">func</span> <span class="titlelight">ComboBox.</span><span class="titlestrong">addItem</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\combobox.swg#L223" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a new item. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">addItem</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, name: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, id: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">, icon: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Icon">Icon</a></span><span class="SyntaxCode"> = {}, userData0: *</span><span class="SyntaxType">void</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">, userData1: *</span><span class="SyntaxType">void</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_ComboBoxItem">ComboBoxItem</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ComboBox_addSeparator"><span class="titletype">func</span> <span class="titlelight">ComboBox.</span><span class="titlestrong">addSeparator</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\combobox.swg#L242" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a separator. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">addSeparator</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ComboBox_clear"><span class="titletype">func</span> <span class="titlelight">ComboBox.</span><span class="titlestrong">clear</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\combobox.swg#L214" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Remove all items. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">clear</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ComboBox_create"><span class="titletype">func</span> <span class="titlelight">ComboBox.</span><span class="titlestrong">create</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\combobox.swg#L192" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">create</span><span class="SyntaxCode">(parent: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode">, position: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode"> = {}, id: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">, kind = </span><span class="SyntaxConstant"><a href="#Gui_ComboBoxKind">ComboBoxKind</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Select</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_ComboBox">ComboBox</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ComboBox_getSelectedItem"><span class="titletype">func</span> <span class="titlelight">ComboBox.</span><span class="titlestrong">getSelectedItem</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\combobox.swg#L250" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the selected item. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getSelectedItem</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_ComboBoxItem">ComboBoxItem</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ComboBox_selectItem"><span class="titletype">func</span> <span class="titlelight">ComboBox.</span><span class="titlestrong">selectItem</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\combobox.swg#L258" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the selected item index. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">selectItem</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, idx: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ComboBoxItem"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">ComboBoxItem</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\combobox.swg#L11" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> base</td>
<td><a href="#Gui_Item">Gui.Item</a></td>
<td></td>
</tr>
<tr>
<td>isSeparator</td>
<td>bool</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ComboBoxKind"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">ComboBoxKind</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\combobox.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>Select</td>
<td></td>
</tr>
<tr>
<td>Edit</td>
<td></td>
</tr>
<tr>
<td>CheckBox</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ComboCtrl"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">ComboCtrl</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\comboctrl.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> frameWnd</td>
<td><a href="#Gui_FrameWnd">Gui.FrameWnd</a></td>
<td></td>
</tr>
<tr>
<td>labelSize</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>comboSize</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>label</td>
<td>*Gui.Label</td>
<td></td>
</tr>
<tr>
<td>combo</td>
<td>*Gui.ComboBox</td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_ComboCtrl_create">create(*Wnd, string, const ref Rectangle, WndId)</a></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ComboCtrl_IWnd_onResizeEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onResizeEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\comboctrl.swg#L16" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onResizeEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_ResizeEvent">ResizeEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ComboCtrl_create"><span class="titletype">func</span> <span class="titlelight">ComboCtrl.</span><span class="titlestrong">create</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\comboctrl.swg#L31" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">create</span><span class="SyntaxCode">(parent: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode">, name: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, position: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode"> = {}, id: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_ComboCtrl">ComboCtrl</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_CommandEvent"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">CommandEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\events.swg#L69" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> base</td>
<td><a href="#Gui_Event">Gui.Event</a></td>
<td></td>
</tr>
<tr>
<td>id</td>
<td>Gui.WndId</td>
<td></td>
</tr>
<tr>
<td>source</td>
<td>*Gui.Wnd</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_CommandStateEvent"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">CommandStateEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\events.swg#L76" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> base</td>
<td><a href="#Gui_Event">Gui.Event</a></td>
<td></td>
</tr>
<tr>
<td>id</td>
<td>Gui.WndId</td>
<td></td>
</tr>
<tr>
<td>source</td>
<td>*Gui.Wnd</td>
<td></td>
</tr>
<tr>
<td>icon</td>
<td><a href="#Gui_Icon">Gui.Icon</a></td>
<td></td>
</tr>
<tr>
<td>name</td>
<td>Core.String</td>
<td></td>
</tr>
<tr>
<td>longName</td>
<td>Core.String</td>
<td></td>
</tr>
<tr>
<td>shortcut</td>
<td>Core.String</td>
<td></td>
</tr>
<tr>
<td>toolTip</td>
<td>Core.String</td>
<td></td>
</tr>
<tr>
<td>setFlags</td>
<td>Gui.CommandStateEvent.SetFlags</td>
<td></td>
</tr>
<tr>
<td>disabled</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>checked</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>hidden</td>
<td>bool</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_CreateEvent"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">CreateEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\events.swg#L119" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> base</td>
<td><a href="#Gui_Event">Gui.Event</a></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Cursor"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">Cursor</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\cursor.swg#L20" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> native</td>
<td><a href="#Gui_NativeCursor">Gui.NativeCursor</a></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_Cursor_apply">apply(const ref Cursor)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Cursor_clear">clear(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Cursor_from">from(CursorShape)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Cursor_wait">wait()</a></td>
<td>Force the wait cursor. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Cursor_apply"><span class="titletype">func</span> <span class="titlelight">Cursor.</span><span class="titlestrong">apply</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\cursor.win32.swg#L43" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">apply</span><span class="SyntaxCode">(cursor: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Cursor">Cursor</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Cursor_clear"><span class="titletype">func</span> <span class="titlelight">Cursor.</span><span class="titlestrong">clear</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\cursor.win32.swg#L38" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">clear</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Cursor_from"><span class="titletype">func</span> <span class="titlelight">Cursor.</span><span class="titlestrong">from</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\cursor.win32.swg#L33" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">from</span><span class="SyntaxCode">(shape: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_CursorShape">CursorShape</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Cursor">Cursor</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Cursor_wait"><span class="titletype">func</span> <span class="titlelight">Cursor.</span><span class="titlestrong">wait</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\cursor.swg#L28" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Force the wait cursor. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">wait</span><span class="SyntaxCode">()</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_CursorShape"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">CursorShape</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\cursor.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>Arrow</td>
<td></td>
</tr>
<tr>
<td>SizeWE</td>
<td></td>
</tr>
<tr>
<td>SizeNS</td>
<td></td>
</tr>
<tr>
<td>SizeNWSE</td>
<td></td>
</tr>
<tr>
<td>SizeNESW</td>
<td></td>
</tr>
<tr>
<td>SizeAll</td>
<td></td>
</tr>
<tr>
<td>Cross</td>
<td></td>
</tr>
<tr>
<td>Help</td>
<td></td>
</tr>
<tr>
<td>Hand</td>
<td></td>
</tr>
<tr>
<td>IBeam</td>
<td></td>
</tr>
<tr>
<td>No</td>
<td></td>
</tr>
<tr>
<td>Wait</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_DestroyEvent"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">DestroyEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\events.swg#L129" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> base</td>
<td><a href="#Gui_Event">Gui.Event</a></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Dialog"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">Dialog</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\dialog.swg#L3" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> wnd</td>
<td><a href="#Gui_Wnd">Gui.Wnd</a></td>
<td></td>
</tr>
<tr>
<td>wndBottom</td>
<td>*Gui.StackLayoutCtrl</td>
<td></td>
</tr>
<tr>
<td>buttons</td>
<td>Core.Array'(*Gui.PushButton)</td>
<td></td>
</tr>
<tr>
<td>sigPressedButton</td>
<td>closure(*Gui.Dialog, Gui.WndId)->bool</td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_Dialog_addButton">addButton(self, string, WndId, bool)</a></td>
<td>Add a button. </td>
</tr>
<tr>
<td><a href="#Gui_Dialog_createDialog">createDialog(*Surface, s32, s32, string)</a></td>
<td>Creates a simple dialog box. </td>
</tr>
<tr>
<td><a href="#Gui_Dialog_createSurface">createSurface(self, *Surface, s32, s32, s32, s32, SurfaceFlags)</a></td>
<td>Creates an associated surface. </td>
</tr>
<tr>
<td><a href="#Gui_Dialog_doModal">doModal(self)</a></td>
<td>Display dialog as modal, and returns the user selected window id. </td>
</tr>
<tr>
<td><a href="#Gui_Dialog_validateId">validateId(self, WndId)</a></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Dialog_IWnd_onKeyEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onKeyEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\dialog.swg#L20" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onKeyEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_KeyEvent">KeyEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Dialog_IWnd_onSysCommandEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onSysCommandEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\dialog.swg#L13" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onSysCommandEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_SysCommandEvent">SysCommandEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Dialog_addButton"><span class="titletype">func</span> <span class="titlelight">Dialog.</span><span class="titlestrong">addButton</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\dialog.swg#L133" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a button. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">addButton</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, str: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, id: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode">, isDefault: </span><span class="SyntaxType">bool</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_PushButton">PushButton</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Dialog_createDialog"><span class="titletype">func</span> <span class="titlelight">Dialog.</span><span class="titlestrong">createDialog</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\dialog.swg#L90" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Creates a simple dialog box. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">createDialog</span><span class="SyntaxCode">(from: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Surface">Surface</a></span><span class="SyntaxCode">, width = </span><span class="SyntaxNumber">512</span><span class="SyntaxCode">, height = </span><span class="SyntaxNumber">300</span><span class="SyntaxCode">, title: </span><span class="SyntaxType">string</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Dialog">Dialog</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Dialog_createSurface"><span class="titletype">func</span> <span class="titlelight">Dialog.</span><span class="titlestrong">createSurface</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\dialog.swg#L100" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Creates an associated surface. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">createSurface</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, from: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Surface">Surface</a></span><span class="SyntaxCode">, x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, width = </span><span class="SyntaxNumber">512</span><span class="SyntaxCode">, height = </span><span class="SyntaxNumber">300</span><span class="SyntaxCode">, flags = </span><span class="SyntaxConstant"><a href="#Gui_SurfaceFlags">SurfaceFlags</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Zero</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Surface">Surface</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Dialog_doModal"><span class="titletype">func</span> <span class="titlelight">Dialog.</span><span class="titlestrong">doModal</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\dialog.swg#L124" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Display dialog as modal, and returns the user selected window id. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">doModal</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxType">string</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Dialog_validateId"><span class="titletype">func</span> <span class="titlelight">Dialog.</span><span class="titlestrong">validateId</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\dialog.swg#L153" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">validateId</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, btnId: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_DockStyle"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">DockStyle</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L7" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>None</td>
<td></td>
</tr>
<tr>
<td>Top</td>
<td></td>
</tr>
<tr>
<td>Left</td>
<td></td>
</tr>
<tr>
<td>Right</td>
<td></td>
</tr>
<tr>
<td>Bottom</td>
<td></td>
</tr>
<tr>
<td>Center</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_DwmExtendFrameIntoClientArea"><span class="titletype">func</span> <span class="titlelight">Gui.</span><span class="titlestrong">DwmExtendFrameIntoClientArea</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.win32.swg#L17" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">DwmExtendFrameIntoClientArea</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, pMarInset: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">MARGINS</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_EditBox"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">EditBox</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\editbox.swg#L37" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> wnd</td>
<td><a href="#Gui_Wnd">Gui.Wnd</a></td>
<td></td>
</tr>
<tr>
<td>icon</td>
<td><a href="#Gui_Icon">Gui.Icon</a></td>
<td></td>
</tr>
<tr>
<td>text</td>
<td>Core.String</td>
<td></td>
</tr>
<tr>
<td>form</td>
<td><a href="#Gui_EditBoxForm">Gui.EditBoxForm</a></td>
<td></td>
</tr>
<tr>
<td>editBoxFlags</td>
<td><a href="#Gui_EditBoxFlags">Gui.EditBoxFlags</a></td>
<td></td>
</tr>
<tr>
<td>inputMode</td>
<td><a href="#Gui_EditBoxInputMode">Gui.EditBoxInputMode</a></td>
<td></td>
</tr>
<tr>
<td>maxLength</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>rightMargin</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>sigChanged</td>
<td>Gui.SigArray'(closure(*Gui.EditBox))</td>
<td></td>
</tr>
<tr>
<td>sigLoseFocus</td>
<td>Gui.SigArray'(closure(*Gui.EditBox))</td>
<td></td>
</tr>
<tr>
<td>sigEnterPressed</td>
<td>Gui.SigArray'(closure(*Gui.EditBox))</td>
<td></td>
</tr>
<tr>
<td>sigEscapePressed</td>
<td>Gui.SigArray'(closure(*Gui.EditBox))</td>
<td></td>
</tr>
<tr>
<td>sigCheckContent</td>
<td>closure(*Gui.EditBox)->Gui.EditBoxCheckResult</td>
<td></td>
</tr>
<tr>
<td><b>using</b> minMax</td>
<td>{minMaxS64: {min: s64, max: s64}, minMaxU64: {min: u64, max: u64}, minMaxF64: {min: f64, max: f64}}</td>
<td></td>
</tr>
<tr>
<td>aniBk</td>
<td><a href="#Gui_BlendColor">Gui.BlendColor</a></td>
<td></td>
</tr>
<tr>
<td>aniBorder</td>
<td><a href="#Gui_BlendColor">Gui.BlendColor</a></td>
<td></td>
</tr>
<tr>
<td>isInvalid</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>isHot</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>isDragging</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>selBeg</td>
<td>u64</td>
<td></td>
</tr>
<tr>
<td>selEnd</td>
<td>u64</td>
<td></td>
</tr>
<tr>
<td>timerCaret</td>
<td>*Gui.Timer</td>
<td></td>
</tr>
<tr>
<td>visibleCaret</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>posSelBeg</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>posSelEnd</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>scrollPosX</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>countRunes</td>
<td>s32</td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_EditBox_create">create(*Wnd, string, const ref Rectangle, WndId)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_EditBox_deleteSelection">deleteSelection(self)</a></td>
<td>Delete selected text. </td>
</tr>
<tr>
<td><a href="#Gui_EditBox_getText">getText(self)</a></td>
<td>Get the associated text. </td>
</tr>
<tr>
<td><a href="#Gui_EditBox_selectAll">selectAll(self)</a></td>
<td>Select all text. </td>
</tr>
<tr>
<td><a href="#Gui_EditBox_setForm">setForm(self, EditBoxForm)</a></td>
<td>Set the editbox form. </td>
</tr>
<tr>
<td><a href="#Gui_EditBox_setInputF64">setInputF64(self, f64, f64)</a></td>
<td>Editbox will edit floating points. </td>
</tr>
<tr>
<td><a href="#Gui_EditBox_setInputS64">setInputS64(self, s64, s64)</a></td>
<td>Editbox will edit signed integers. </td>
</tr>
<tr>
<td><a href="#Gui_EditBox_setInputU64">setInputU64(self, u64, u64)</a></td>
<td>Editbox will edit unsigned integers. </td>
</tr>
<tr>
<td><a href="#Gui_EditBox_setNote">setNote(self, string)</a></td>
<td>Set the editbox note. </td>
</tr>
<tr>
<td><a href="#Gui_EditBox_setSelection">setSelection(self, u64, u64)</a></td>
<td>Set current selection. </td>
</tr>
<tr>
<td><a href="#Gui_EditBox_setText">setText(self, string)</a></td>
<td>Set the editbox content. </td>
</tr>
<tr>
<td><a href="#Gui_EditBox_setTextSilent">setTextSilent(self, string)</a></td>
<td>Set the editbox content, but do not notify change. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_EditBox_IWnd_onFocusEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onFocusEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\editbox.swg#L78" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onFocusEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_FocusEvent">FocusEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_EditBox_IWnd_onKeyEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onKeyEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\editbox.swg#L287" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onKeyEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_KeyEvent">KeyEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_EditBox_IWnd_onMouseEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onMouseEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\editbox.swg#L228" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onMouseEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_MouseEvent">MouseEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_EditBox_IWnd_onPaintEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onPaintEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\editbox.swg#L97" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onPaintEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_PaintEvent">PaintEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_EditBox_IWnd_onTimerEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onTimerEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\editbox.swg#L279" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onTimerEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_TimerEvent">TimerEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_EditBox_create"><span class="titletype">func</span> <span class="titlelight">EditBox.</span><span class="titlestrong">create</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\editbox.swg#L795" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">create</span><span class="SyntaxCode">(parent: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode">, content: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, position: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode"> = {}, id: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_EditBox">EditBox</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_EditBox_deleteSelection"><span class="titletype">func</span> <span class="titlelight">EditBox.</span><span class="titlestrong">deleteSelection</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\editbox.swg#L812" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Delete selected text. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">deleteSelection</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_EditBox_getText"><span class="titletype">func</span> <span class="titlelight">EditBox.</span><span class="titlestrong">getText</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\editbox.swg#L863" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the associated text. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getText</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxType">string</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_EditBox_selectAll"><span class="titletype">func</span> <span class="titlelight">EditBox.</span><span class="titlestrong">selectAll</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\editbox.swg#L830" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Select all text. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">selectAll</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_EditBox_setForm"><span class="titletype">func</span> <span class="titlelight">EditBox.</span><span class="titlestrong">setForm</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\editbox.swg#L893" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the editbox form. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setForm</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, form: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_EditBoxForm">EditBoxForm</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_EditBox_setInputF64"><span class="titletype">func</span> <span class="titlelight">EditBox.</span><span class="titlestrong">setInputF64</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\editbox.swg#L885" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Editbox will edit floating points. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setInputF64</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, min = -</span><span class="SyntaxConstant">Swag</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">F64</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Max</span><span class="SyntaxCode">, max = </span><span class="SyntaxConstant">Swag</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">F64</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Max</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_EditBox_setInputS64"><span class="titletype">func</span> <span class="titlelight">EditBox.</span><span class="titlestrong">setInputS64</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\editbox.swg#L869" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Editbox will edit signed integers. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setInputS64</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, min = </span><span class="SyntaxConstant">Swag</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">S64</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Min</span><span class="SyntaxCode">, max = </span><span class="SyntaxConstant">Swag</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">S64</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Max</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_EditBox_setInputU64"><span class="titletype">func</span> <span class="titlelight">EditBox.</span><span class="titlestrong">setInputU64</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\editbox.swg#L877" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Editbox will edit unsigned integers. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setInputU64</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, min = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">'</span><span class="SyntaxType">u64</span><span class="SyntaxCode">, max = </span><span class="SyntaxConstant">Swag</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">U64</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Max</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_EditBox_setNote"><span class="titletype">func</span> <span class="titlelight">EditBox.</span><span class="titlestrong">setNote</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\editbox.swg#L836" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the editbox note. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setNote</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, note: </span><span class="SyntaxType">string</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_EditBox_setSelection"><span class="titletype">func</span> <span class="titlelight">EditBox.</span><span class="titlestrong">setSelection</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\editbox.swg#L821" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set current selection. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setSelection</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, start: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">, end: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_EditBox_setText"><span class="titletype">func</span> <span class="titlelight">EditBox.</span><span class="titlestrong">setText</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\editbox.swg#L851" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the editbox content. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setText</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, str: </span><span class="SyntaxType">string</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_EditBox_setTextSilent"><span class="titletype">func</span> <span class="titlelight">EditBox.</span><span class="titlestrong">setTextSilent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\editbox.swg#L843" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the editbox content, but do not notify change. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setTextSilent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, str: </span><span class="SyntaxType">string</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_EditBoxCheckResult"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">EditBoxCheckResult</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\editbox.swg#L20" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>Incomplete</td>
<td></td>
</tr>
<tr>
<td>Valid</td>
<td></td>
</tr>
<tr>
<td>InvalidDismiss</td>
<td></td>
</tr>
<tr>
<td>InvalidShow</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_EditBoxFlags"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">EditBoxFlags</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\editbox.swg#L29" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>Zero</td>
<td></td>
</tr>
<tr>
<td>AutoLoseFocus</td>
<td></td>
</tr>
<tr>
<td>ReadOnly</td>
<td></td>
</tr>
<tr>
<td>RightAlign</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_EditBoxForm"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">EditBoxForm</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\editbox.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>Square</td>
<td></td>
</tr>
<tr>
<td>Round</td>
<td></td>
</tr>
<tr>
<td>Flat</td>
<td></td>
</tr>
<tr>
<td>Transparent</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_EditBoxInputMode"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">EditBoxInputMode</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\editbox.swg#L12" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>String</td>
<td></td>
</tr>
<tr>
<td>S64</td>
<td></td>
</tr>
<tr>
<td>U64</td>
<td></td>
</tr>
<tr>
<td>F64</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_EditCtrl"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">EditCtrl</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\editctrl.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> frameWnd</td>
<td><a href="#Gui_FrameWnd">Gui.FrameWnd</a></td>
<td></td>
</tr>
<tr>
<td>labelSize</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>editSize</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>label</td>
<td>*Gui.Label</td>
<td></td>
</tr>
<tr>
<td>edit</td>
<td>*Gui.EditBox</td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_EditCtrl_create">create(*Wnd, string, string, const ref Rectangle, WndId, EditBoxFlags)</a></td>
<td>Create the popup list, but do not display it. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_EditCtrl_IWnd_onResizeEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onResizeEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\editctrl.swg#L16" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onResizeEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_ResizeEvent">ResizeEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_EditCtrl_create"><span class="titletype">func</span> <span class="titlelight">EditCtrl.</span><span class="titlestrong">create</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\editctrl.swg#L32" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Create the popup list, but do not display it. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">create</span><span class="SyntaxCode">(parent: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode">, name: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, text: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, position: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode"> = {}, id: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">, flags: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_EditBoxFlags">EditBoxFlags</a></span><span class="SyntaxCode"> = </span><span class="SyntaxConstant">Zero</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_EditCtrl">EditCtrl</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_EditDlg"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">EditDlg</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\editdlg.swg#L3" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> dialog</td>
<td><a href="#Gui_Dialog">Gui.Dialog</a></td>
<td></td>
</tr>
<tr>
<td>label</td>
<td>*Gui.Label</td>
<td></td>
</tr>
<tr>
<td>edit</td>
<td>*Gui.EditBox</td>
<td></td>
</tr>
<tr>
<td>richEdit</td>
<td>*Gui.RichEditCtrl</td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_EditDlg_createMultiLine">createMultiLine(*Surface, s32)</a></td>
<td>Creates the message box. </td>
</tr>
<tr>
<td><a href="#Gui_EditDlg_createSingleLine">createSingleLine(*Surface)</a></td>
<td>Creates the message box. </td>
</tr>
<tr>
<td><a href="#Gui_EditDlg_getText">getText(self)</a></td>
<td>Get the text. </td>
</tr>
<tr>
<td><a href="#Gui_EditDlg_setMessage">setMessage(self, string, s32)</a></td>
<td>Associate a message above the editbox. </td>
</tr>
<tr>
<td><a href="#Gui_EditDlg_setText">setText(self, string)</a></td>
<td>Set the text. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_EditDlg_createMultiLine"><span class="titletype">func</span> <span class="titlelight">EditDlg.</span><span class="titlestrong">createMultiLine</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\editdlg.swg#L38" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Creates the message box. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">createMultiLine</span><span class="SyntaxCode">(from: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Surface">Surface</a></span><span class="SyntaxCode">, heightEdit = </span><span class="SyntaxNumber">150</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_EditDlg">EditDlg</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_EditDlg_createSingleLine"><span class="titletype">func</span> <span class="titlelight">EditDlg.</span><span class="titlestrong">createSingleLine</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\editdlg.swg#L14" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Creates the message box. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">createSingleLine</span><span class="SyntaxCode">(from: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Surface">Surface</a></span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_EditDlg">EditDlg</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_EditDlg_getText"><span class="titletype">func</span> <span class="titlelight">EditDlg.</span><span class="titlestrong">getText</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\editdlg.swg#L76" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the text. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getText</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">String</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_EditDlg_setMessage"><span class="titletype">func</span> <span class="titlelight">EditDlg.</span><span class="titlestrong">setMessage</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\editdlg.swg#L84" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Associate a message above the editbox. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setMessage</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, message: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, height: </span><span class="SyntaxType">s32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">40</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_EditDlg_setText"><span class="titletype">func</span> <span class="titlelight">EditDlg.</span><span class="titlestrong">setText</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\editdlg.swg#L67" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the text. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setText</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, text: </span><span class="SyntaxType">string</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_EmbInfoCtrl"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">EmbInfoCtrl</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\embinfoctrl.swg#L10" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> frameWnd</td>
<td><a href="#Gui_FrameWnd">Gui.FrameWnd</a></td>
<td></td>
</tr>
<tr>
<td>label</td>
<td>*Gui.Label</td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_EmbInfoCtrl_create">create(*Wnd, string, EmbInfoCtrlKind, s32, bool)</a></td>
<td>Create the popup list, but do not display it. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_EmbInfoCtrl_create"><span class="titletype">func</span> <span class="titlelight">EmbInfoCtrl.</span><span class="titlestrong">create</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\embinfoctrl.swg#L19" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Create the popup list, but do not display it. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">create</span><span class="SyntaxCode">(parent: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode">, name: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, kind: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_EmbInfoCtrlKind">EmbInfoCtrlKind</a></span><span class="SyntaxCode">, height = </span><span class="SyntaxNumber">40</span><span class="SyntaxCode">, top = </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_EmbInfoCtrl">EmbInfoCtrl</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_EmbInfoCtrlKind"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">EmbInfoCtrlKind</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\embinfoctrl.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>Critical</td>
<td></td>
</tr>
<tr>
<td>Information</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Event"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">Event</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\events.swg#L43" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>kind</td>
<td><a href="#Gui_EventKind">Gui.EventKind</a></td>
<td></td>
</tr>
<tr>
<td>type</td>
<td>const *Swag.TypeInfo</td>
<td></td>
</tr>
<tr>
<td>target</td>
<td>*Gui.Wnd</td>
<td></td>
</tr>
<tr>
<td>accepted</td>
<td>bool</td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_Event_create">create()</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Event_create">create(EventKind)</a></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Event_create"><span class="titletype">func</span> <span class="titlelight">Event.</span><span class="titlestrong">create</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\events.swg#L53" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">T</span><span class="SyntaxCode">) </span><span class="SyntaxFunction">create</span><span class="SyntaxCode">()-&gt;*</span><span class="SyntaxConstant">T</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">T</span><span class="SyntaxCode">) </span><span class="SyntaxFunction">create</span><span class="SyntaxCode">(kind: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_EventKind">EventKind</a></span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">T</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_EventKind"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">EventKind</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\events.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>None</td>
<td></td>
</tr>
<tr>
<td>Create</td>
<td></td>
</tr>
<tr>
<td>Destroy</td>
<td></td>
</tr>
<tr>
<td>Resize</td>
<td></td>
</tr>
<tr>
<td>Move</td>
<td></td>
</tr>
<tr>
<td>PrePaint</td>
<td></td>
</tr>
<tr>
<td>Paint</td>
<td></td>
</tr>
<tr>
<td>PostPaint</td>
<td></td>
</tr>
<tr>
<td>KeyPressed</td>
<td></td>
</tr>
<tr>
<td>KeyReleased</td>
<td></td>
</tr>
<tr>
<td>Rune</td>
<td></td>
</tr>
<tr>
<td>MouseAccept</td>
<td></td>
</tr>
<tr>
<td>MousePressed</td>
<td></td>
</tr>
<tr>
<td>MouseReleased</td>
<td></td>
</tr>
<tr>
<td>MouseDoubleClick</td>
<td></td>
</tr>
<tr>
<td>MouseMove</td>
<td></td>
</tr>
<tr>
<td>MouseEnter</td>
<td></td>
</tr>
<tr>
<td>MouseLeave</td>
<td></td>
</tr>
<tr>
<td>MouseWheel</td>
<td></td>
</tr>
<tr>
<td>Quit</td>
<td></td>
</tr>
<tr>
<td>SysCommand</td>
<td></td>
</tr>
<tr>
<td>SetTheme</td>
<td></td>
</tr>
<tr>
<td>Timer</td>
<td></td>
</tr>
<tr>
<td>SetFocus</td>
<td></td>
</tr>
<tr>
<td>KillFocus</td>
<td></td>
</tr>
<tr>
<td>Command</td>
<td></td>
</tr>
<tr>
<td>ComputeCommandState</td>
<td></td>
</tr>
<tr>
<td>ApplyCommandState</td>
<td></td>
</tr>
<tr>
<td>Frame</td>
<td></td>
</tr>
<tr>
<td>SerializeState</td>
<td></td>
</tr>
<tr>
<td>Show</td>
<td></td>
</tr>
<tr>
<td>Hide</td>
<td></td>
</tr>
<tr>
<td>Invalidate</td>
<td></td>
</tr>
<tr>
<td>SysUser</td>
<td></td>
</tr>
<tr>
<td>Notify</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_FileDlg"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">FileDlg</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\filedlg.swg#L31" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> dialog</td>
<td><a href="#Gui_Dialog">Gui.Dialog</a></td>
<td></td>
</tr>
<tr>
<td>treeCtrl</td>
<td>*Gui.ListCtrl</td>
<td></td>
</tr>
<tr>
<td>treePathStr</td>
<td>Core.Array'(Core.String)</td>
<td></td>
</tr>
<tr>
<td>listCtrl</td>
<td>*Gui.ListCtrl</td>
<td></td>
</tr>
<tr>
<td>comboPath</td>
<td>*Gui.ComboBox</td>
<td></td>
</tr>
<tr>
<td>comboPathStr</td>
<td>Core.Array'(Core.String)</td>
<td></td>
</tr>
<tr>
<td>editFile</td>
<td>*Gui.EditBox</td>
<td></td>
</tr>
<tr>
<td>comboType</td>
<td>*Gui.ComboBox</td>
<td></td>
</tr>
<tr>
<td>dlgOptions</td>
<td><a href="#Gui_FileDlgOptions">Gui.FileDlgOptions</a></td>
<td></td>
</tr>
<tr>
<td>curFolder</td>
<td>Core.String</td>
<td></td>
</tr>
<tr>
<td>curList</td>
<td>Core.Array'(Core.File.FileInfo)</td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_FileDlg_create">create(*Surface, const ref FileDlgOptions)</a></td>
<td>Creates the message box. </td>
</tr>
<tr>
<td><a href="#Gui_FileDlg_getSelectedName">getSelectedName(self)</a></td>
<td>Get the first selection. </td>
</tr>
<tr>
<td><a href="#Gui_FileDlg_getSelectedNames">getSelectedNames(self)</a></td>
<td>Get all selections. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_FileDlg_IWnd_onDestroyEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onDestroyEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\filedlg.swg#L50" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onDestroyEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_DestroyEvent">DestroyEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_FileDlg_create"><span class="titletype">func</span> <span class="titlelight">FileDlg.</span><span class="titlestrong">create</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\filedlg.swg#L204" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Creates the message box. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">create</span><span class="SyntaxCode">(from: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Surface">Surface</a></span><span class="SyntaxCode">, opt: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_FileDlgOptions">FileDlgOptions</a></span><span class="SyntaxCode"> = {})-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_FileDlg">FileDlg</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_FileDlg_getSelectedName"><span class="titletype">func</span> <span class="titlelight">FileDlg.</span><span class="titlestrong">getSelectedName</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\filedlg.swg#L471" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the first selection. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getSelectedName</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">String</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_FileDlg_getSelectedNames"><span class="titletype">func</span> <span class="titlelight">FileDlg.</span><span class="titlestrong">getSelectedNames</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\filedlg.swg#L480" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get all selections. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getSelectedNames</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Array</span><span class="SyntaxCode">'(</span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">String</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_FileDlgMode"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">FileDlgMode</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\filedlg.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>CreateFile</td>
<td></td>
</tr>
<tr>
<td>SelectOneFile</td>
<td></td>
</tr>
<tr>
<td>SelectMultiFiles</td>
<td></td>
</tr>
<tr>
<td>SelectFolder</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_FileDlgOptions"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">FileDlgOptions</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\filedlg.swg#L18" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>title</td>
<td>string</td>
<td></td>
</tr>
<tr>
<td>btnOkName</td>
<td>string</td>
<td></td>
</tr>
<tr>
<td>openFolder</td>
<td>string</td>
<td></td>
</tr>
<tr>
<td>editName</td>
<td>string</td>
<td></td>
</tr>
<tr>
<td>drivePane</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>mode</td>
<td><a href="#Gui_FileDlgMode">Gui.FileDlgMode</a></td>
<td></td>
</tr>
<tr>
<td>filters</td>
<td>Core.Array'({name: string, extensions: string})</td>
<td></td>
</tr>
<tr>
<td>shortcuts</td>
<td>Core.Array'({type: Core.Env.SpecialDirectory, name: string})</td>
<td></td>
</tr>
<tr>
<td>state</td>
<td>*Gui.FileDlgState</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_FileDlgState"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">FileDlgState</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\filedlg.swg#L12" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>position</td>
<td>Core.Math.Rectangle</td>
<td></td>
</tr>
<tr>
<td>curFolder</td>
<td>Core.String</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_FocusEvent"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">FocusEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\events.swg#L175" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> base</td>
<td><a href="#Gui_Event">Gui.Event</a></td>
<td></td>
</tr>
<tr>
<td>other</td>
<td>*Gui.Wnd</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_FocusStategy"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">FocusStategy</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L36" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>None</td>
<td></td>
</tr>
<tr>
<td>MousePressed</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_FrameEvent"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">FrameEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\events.swg#L113" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> base</td>
<td><a href="#Gui_Event">Gui.Event</a></td>
<td></td>
</tr>
<tr>
<td>firstFrame</td>
<td>bool</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_FrameWnd"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">FrameWnd</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\framewnd.swg#L29" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> wnd</td>
<td><a href="#Gui_Wnd">Gui.Wnd</a></td>
<td></td>
</tr>
<tr>
<td>view</td>
<td>*Gui.Wnd</td>
<td></td>
</tr>
<tr>
<td>frameForm</td>
<td><a href="#Gui_FrameWndForm">Gui.FrameWndForm</a></td>
<td></td>
</tr>
<tr>
<td>frameFlags</td>
<td><a href="#Gui_FrameWndFlags">Gui.FrameWndFlags</a></td>
<td></td>
</tr>
<tr>
<td>anchor</td>
<td><a href="#Gui_FrameWndAnchor">Gui.FrameWndAnchor</a></td>
<td></td>
</tr>
<tr>
<td>anchorPos</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>usedColorBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_FrameWnd_createView">createView(self, HookEvent)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_FrameWnd_setFrameFlags">setFrameFlags(self, FrameWndFlags, FrameWndFlags)</a></td>
<td>Set the frame wnd flags. </td>
</tr>
<tr>
<td><a href="#Gui_FrameWnd_setFrameForm">setFrameForm(self, FrameWndForm)</a></td>
<td>Set the frame wnd form. </td>
</tr>
<tr>
<td><a href="#Gui_FrameWnd_setView">setView(self, *Wnd)</a></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_FrameWnd_IWnd_onPaintEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onPaintEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\framewnd.swg#L60" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onPaintEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_PaintEvent">PaintEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_FrameWnd_IWnd_onPostPaintEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onPostPaintEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\framewnd.swg#L106" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onPostPaintEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_PaintEvent">PaintEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_FrameWnd_IWnd_onPrePaintEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onPrePaintEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\framewnd.swg#L43" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onPrePaintEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_PaintEvent">PaintEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_FrameWnd_IWnd_onResizeEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onResizeEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\framewnd.swg#L207" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onResizeEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_ResizeEvent">ResizeEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_FrameWnd_createView"><span class="titletype">func</span> <span class="titlelight">FrameWnd.</span><span class="titlestrong">createView</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\framewnd.swg#L220" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">T</span><span class="SyntaxCode">) </span><span class="SyntaxFunction">createView</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, hook: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HookEvent</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">T</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_FrameWnd_setFrameFlags"><span class="titletype">func</span> <span class="titlelight">FrameWnd.</span><span class="titlestrong">setFrameFlags</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\framewnd.swg#L238" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the frame wnd flags. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setFrameFlags</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, add = </span><span class="SyntaxConstant"><a href="#Gui_FrameWndFlags">FrameWndFlags</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Zero</span><span class="SyntaxCode">, remove = </span><span class="SyntaxConstant"><a href="#Gui_FrameWndFlags">FrameWndFlags</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Zero</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_FrameWnd_setFrameForm"><span class="titletype">func</span> <span class="titlelight">FrameWnd.</span><span class="titlestrong">setFrameForm</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\framewnd.swg#L255" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the frame wnd form. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setFrameForm</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, form: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_FrameWndForm">FrameWndForm</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_FrameWnd_setView"><span class="titletype">func</span> <span class="titlelight">FrameWnd.</span><span class="titlestrong">setView</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\framewnd.swg#L228" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setView</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, what: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_FrameWndAnchor"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">FrameWndAnchor</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\framewnd.swg#L20" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>None</td>
<td></td>
</tr>
<tr>
<td>Left</td>
<td></td>
</tr>
<tr>
<td>Top</td>
<td></td>
</tr>
<tr>
<td>Right</td>
<td></td>
</tr>
<tr>
<td>Bottom</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_FrameWndFlags"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">FrameWndFlags</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\framewnd.swg#L12" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>Zero</td>
<td></td>
</tr>
<tr>
<td>Borders</td>
<td></td>
</tr>
<tr>
<td>FocusBorder</td>
<td></td>
</tr>
<tr>
<td>SmallShadow</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_FrameWndForm"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">FrameWndForm</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\framewnd.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>Transparent</td>
<td></td>
</tr>
<tr>
<td>Square</td>
<td></td>
</tr>
<tr>
<td>Round</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_GridLayoutCtrl"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">GridLayoutCtrl</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\gridlayoutctrl.swg#L13" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> wnd</td>
<td><a href="#Gui_Wnd">Gui.Wnd</a></td>
<td></td>
</tr>
<tr>
<td>gridLayoutFlags</td>
<td><a href="#Gui_GridLayoutCtrlFlags">Gui.GridLayoutCtrlFlags</a></td>
<td></td>
</tr>
<tr>
<td>numColumns</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>numRows</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>spacingHorz</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>spacingVert</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>layout</td>
<td>Core.Array'(*Gui.Wnd)</td>
<td></td>
</tr>
<tr>
<td>colSizes</td>
<td>Core.Array'(f32)</td>
<td></td>
</tr>
<tr>
<td>rowSizes</td>
<td>Core.Array'(f32)</td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_GridLayoutCtrl_computeLayout">computeLayout(self)</a></td>
<td>Recompute layout of all childs. </td>
</tr>
<tr>
<td><a href="#Gui_GridLayoutCtrl_create">create(*Wnd, s32, s32, const ref Rectangle)</a></td>
<td>Create a layout control. </td>
</tr>
<tr>
<td><a href="#Gui_GridLayoutCtrl_setColRowChild">setColRowChild(self, *Wnd, s32, s32, s32, s32)</a></td>
<td>Set the wnd associated with the given <code class="incode">col</code> and <code class="incode">row</code>. </td>
</tr>
<tr>
<td><a href="#Gui_GridLayoutCtrl_setColSize">setColSize(self, s32, f32)</a></td>
<td>Set size, in pixel, of a given column. </td>
</tr>
<tr>
<td><a href="#Gui_GridLayoutCtrl_setRowSize">setRowSize(self, s32, f32)</a></td>
<td>Set size, in pixel, of a given row. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_GridLayoutCtrl_IWnd_onResizeEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onResizeEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\gridlayoutctrl.swg#L30" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onResizeEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_ResizeEvent">ResizeEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_GridLayoutCtrl_computeLayout"><span class="titletype">func</span> <span class="titlelight">GridLayoutCtrl.</span><span class="titlestrong">computeLayout</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\gridlayoutctrl.swg#L138" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Recompute layout of all childs. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">computeLayout</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_GridLayoutCtrl_create"><span class="titletype">func</span> <span class="titlelight">GridLayoutCtrl.</span><span class="titlestrong">create</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\gridlayoutctrl.swg#L120" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Create a layout control. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">create</span><span class="SyntaxCode">(parent: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode">, numColumns: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, numRows: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, position: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode"> = {})-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_GridLayoutCtrl">GridLayoutCtrl</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_GridLayoutCtrl_setColRowChild"><span class="titletype">func</span> <span class="titlelight">GridLayoutCtrl.</span><span class="titlestrong">setColRowChild</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\gridlayoutctrl.swg#L157" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the wnd associated with the given <code class="incode">col</code> and <code class="incode">row</code>. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setColRowChild</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, child: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode">, col: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, row: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, spanH: </span><span class="SyntaxType">s32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">, spanV: </span><span class="SyntaxType">s32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)</code>
</pre>
<p> Window can cover multiple columns and rows. </p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_GridLayoutCtrl_setColSize"><span class="titletype">func</span> <span class="titlelight">GridLayoutCtrl.</span><span class="titlestrong">setColSize</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\gridlayoutctrl.swg#L144" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set size, in pixel, of a given column. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setColSize</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, col: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, size: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_GridLayoutCtrl_setRowSize"><span class="titletype">func</span> <span class="titlelight">GridLayoutCtrl.</span><span class="titlestrong">setRowSize</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\gridlayoutctrl.swg#L150" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set size, in pixel, of a given row. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setRowSize</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, row: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, size: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_GridLayoutCtrlFlags"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">GridLayoutCtrlFlags</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\gridlayoutctrl.swg#L5" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>Zero</td>
<td></td>
</tr>
<tr>
<td>AdaptSizeToContent</td>
<td></td>
</tr>
<tr>
<td>AdaptColWidth</td>
<td></td>
</tr>
<tr>
<td>AdaptRowHeight</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Header"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">Header</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\header.swg#L31" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> wnd</td>
<td><a href="#Gui_Wnd">Gui.Wnd</a></td>
<td></td>
</tr>
<tr>
<td>form</td>
<td><a href="#Gui_HeaderForm">Gui.HeaderForm</a></td>
<td></td>
</tr>
<tr>
<td>headerFlags</td>
<td><a href="#Gui_HeaderFlags">Gui.HeaderFlags</a></td>
<td></td>
</tr>
<tr>
<td>marginItems</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>sigLayoutChanged</td>
<td>closure(*Gui.Header)</td>
<td></td>
</tr>
<tr>
<td>sigClicked</td>
<td>closure(*Gui.Header, u32)</td>
<td></td>
</tr>
<tr>
<td>hotIdx</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>hotSeparator</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>moving</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>clicked</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>correctMoving</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>sortColumn</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>sortMark</td>
<td>s32</td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_Header_addItem">addItem(self, string, f32, const ref Icon)</a></td>
<td>Add a new view. </td>
</tr>
<tr>
<td><a href="#Gui_Header_create">create(*Wnd, const ref Rectangle, WndId)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Header_getItem">getItem(self, u32)</a></td>
<td>Returns the given item, by index. </td>
</tr>
<tr>
<td><a href="#Gui_Header_getItemByCol">getItemByCol(self, u32)</a></td>
<td>Returns the given item, by column. </td>
</tr>
<tr>
<td><a href="#Gui_Header_getItemPosition">getItemPosition(self, u32)</a></td>
<td>Get an item position by index. </td>
</tr>
<tr>
<td><a href="#Gui_Header_getItemPositionByCol">getItemPositionByCol(self, u32)</a></td>
<td>Get an item position by column. </td>
</tr>
<tr>
<td><a href="#Gui_Header_getItemWidth">getItemWidth(self, u32)</a></td>
<td>Get an item width by index. </td>
</tr>
<tr>
<td><a href="#Gui_Header_getItemWidthByCol">getItemWidthByCol(self, u32)</a></td>
<td>Get an item width by column. </td>
</tr>
<tr>
<td><a href="#Gui_Header_getTotalItemsWidth">getTotalItemsWidth(self)</a></td>
<td>Get the header total width. </td>
</tr>
<tr>
<td><a href="#Gui_Header_setItemWidth">setItemWidth(self, u32, f32)</a></td>
<td>Set an item width by index. </td>
</tr>
<tr>
<td><a href="#Gui_Header_setItemWidthByCol">setItemWidthByCol(self, u32, f32)</a></td>
<td>Set an item width by column. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Header_IWnd_onMouseEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onMouseEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\header.swg#L158" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onMouseEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_MouseEvent">MouseEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Header_IWnd_onPaintEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onPaintEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\header.swg#L114" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onPaintEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_PaintEvent">PaintEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Header_IWnd_paintItem"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">paintItem</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\header.swg#L53" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">paintItem</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, bc: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_PaintContext">PaintContext</a></span><span class="SyntaxCode">, item: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_HeaderItem">HeaderItem</a></span><span class="SyntaxCode">, idx: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">, x: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Header_addItem"><span class="titletype">func</span> <span class="titlelight">Header.</span><span class="titlestrong">addItem</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\header.swg#L297" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a new view. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">addItem</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, tabName: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, width = </span><span class="SyntaxNumber">100</span><span class="SyntaxCode">'</span><span class="SyntaxType">f32</span><span class="SyntaxCode">, icon: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Icon">Icon</a></span><span class="SyntaxCode"> = {})-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_HeaderItem">HeaderItem</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Header_create"><span class="titletype">func</span> <span class="titlelight">Header.</span><span class="titlestrong">create</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\header.swg#L281" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">create</span><span class="SyntaxCode">(parent: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode">, position: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode"> = {}, id: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Header">Header</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Header_getItem"><span class="titletype">func</span> <span class="titlelight">Header.</span><span class="titlestrong">getItem</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\header.swg#L309" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the given item, by index. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getItem</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, idx: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_HeaderItem">HeaderItem</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Header_getItemByCol"><span class="titletype">func</span> <span class="titlelight">Header.</span><span class="titlestrong">getItemByCol</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\header.swg#L316" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the given item, by column. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getItemByCol</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, column: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_HeaderItem">HeaderItem</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Header_getItemPosition"><span class="titletype">func</span> <span class="titlelight">Header.</span><span class="titlestrong">getItemPosition</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\header.swg#L351" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get an item position by index. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getItemPosition</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, idx: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Header_getItemPositionByCol"><span class="titletype">func</span> <span class="titlelight">Header.</span><span class="titlestrong">getItemPositionByCol</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\header.swg#L358" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get an item position by column. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getItemPositionByCol</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, column: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Header_getItemWidth"><span class="titletype">func</span> <span class="titlelight">Header.</span><span class="titlestrong">getItemWidth</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\header.swg#L330" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get an item width by index. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getItemWidth</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, idx: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Header_getItemWidthByCol"><span class="titletype">func</span> <span class="titlelight">Header.</span><span class="titlestrong">getItemWidthByCol</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\header.swg#L337" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get an item width by column. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getItemWidthByCol</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, column: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Header_getTotalItemsWidth"><span class="titletype">func</span> <span class="titlelight">Header.</span><span class="titlestrong">getTotalItemsWidth</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\header.swg#L375" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the header total width. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getTotalItemsWidth</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Header_setItemWidth"><span class="titletype">func</span> <span class="titlelight">Header.</span><span class="titlestrong">setItemWidth</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\header.swg#L384" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set an item width by index. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setItemWidth</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, idx: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">, width: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Header_setItemWidthByCol"><span class="titletype">func</span> <span class="titlelight">Header.</span><span class="titlestrong">setItemWidthByCol</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\header.swg#L391" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set an item width by column. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setItemWidthByCol</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, column: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">, width: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_HeaderFlags"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">HeaderFlags</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\header.swg#L25" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>Zero</td>
<td></td>
</tr>
<tr>
<td>Clickable</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_HeaderForm"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">HeaderForm</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\header.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>Transparent</td>
<td></td>
</tr>
<tr>
<td>Flat</td>
<td></td>
</tr>
<tr>
<td>Round</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_HeaderItem"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">HeaderItem</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\header.swg#L11" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>name</td>
<td>Pixel.RichString</td>
<td></td>
</tr>
<tr>
<td>icon</td>
<td><a href="#Gui_Icon">Gui.Icon</a></td>
<td></td>
</tr>
<tr>
<td>width</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>minWidth</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>maxWidth</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>sizeable</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>clickable</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>iconMargin</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>column</td>
<td>u32</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_IActionUI"><span class="titletype">interface</span> <span class="titlelight">Gui.</span><span class="titlestrong">IActionUI</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\action.swg#L7" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>accept</td>
<td>func(*Gui.IActionUI, Gui.ActionContext)->bool</td>
<td></td>
</tr>
<tr>
<td>update</td>
<td>func(*Gui.IActionUI, Gui.ActionContext, *Gui.CommandStateEvent)->bool</td>
<td></td>
</tr>
<tr>
<td>execute</td>
<td>func(*Gui.IActionUI, Gui.ActionContext)->bool</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_IRichEditLexer"><span class="titletype">interface</span> <span class="titlelight">Gui.</span><span class="titlestrong">IRichEditLexer</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditlexer.swg#L1" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>setup</td>
<td>func(*Gui.IRichEditLexer, *Gui.RichEditCtrl)</td>
<td></td>
</tr>
<tr>
<td>insertRune</td>
<td>func(*Gui.IRichEditLexer, *Gui.RichEditCtrl, rune)->bool</td>
<td></td>
</tr>
<tr>
<td>compute</td>
<td>func(*Gui.IRichEditLexer, *Gui.RichEditLine, *Gui.RichEditLine, *Gui.RichEditLine)</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_IWnd"><span class="titletype">interface</span> <span class="titlelight">Gui.</span><span class="titlestrong">IWnd</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L50" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>onEvent</td>
<td>func(*Gui.IWnd, *Gui.Event)</td>
<td></td>
</tr>
<tr>
<td>onHookEvent</td>
<td>func(*Gui.IWnd, *Gui.Event)</td>
<td></td>
</tr>
<tr>
<td>onCreateEvent</td>
<td>func(*Gui.IWnd, *Gui.CreateEvent)</td>
<td></td>
</tr>
<tr>
<td>onDestroyEvent</td>
<td>func(*Gui.IWnd, *Gui.DestroyEvent)</td>
<td></td>
</tr>
<tr>
<td>onStateEvent</td>
<td>func(*Gui.IWnd, *Gui.StateEvent)</td>
<td></td>
</tr>
<tr>
<td>onResizeEvent</td>
<td>func(*Gui.IWnd, *Gui.ResizeEvent)</td>
<td></td>
</tr>
<tr>
<td>onPrePaintEvent</td>
<td>func(*Gui.IWnd, *Gui.PaintEvent)</td>
<td></td>
</tr>
<tr>
<td>onPaintEvent</td>
<td>func(*Gui.IWnd, *Gui.PaintEvent)</td>
<td></td>
</tr>
<tr>
<td>onPostPaintEvent</td>
<td>func(*Gui.IWnd, *Gui.PaintEvent)</td>
<td></td>
</tr>
<tr>
<td>onKeyEvent</td>
<td>func(*Gui.IWnd, *Gui.KeyEvent)</td>
<td></td>
</tr>
<tr>
<td>onMouseEvent</td>
<td>func(*Gui.IWnd, *Gui.MouseEvent)</td>
<td></td>
</tr>
<tr>
<td>onSysCommandEvent</td>
<td>func(*Gui.IWnd, *Gui.SysCommandEvent)</td>
<td></td>
</tr>
<tr>
<td>onSysUserEvent</td>
<td>func(*Gui.IWnd, *Gui.SysUserEvent)</td>
<td></td>
</tr>
<tr>
<td>onSetThemeEvent</td>
<td>func(*Gui.IWnd, *Gui.SetThemeEvent)</td>
<td></td>
</tr>
<tr>
<td>onTimerEvent</td>
<td>func(*Gui.IWnd, *Gui.TimerEvent)</td>
<td></td>
</tr>
<tr>
<td>onFocusEvent</td>
<td>func(*Gui.IWnd, *Gui.FocusEvent)</td>
<td></td>
</tr>
<tr>
<td>onCommandEvent</td>
<td>func(*Gui.IWnd, *Gui.CommandEvent)</td>
<td></td>
</tr>
<tr>
<td>onComputeStateEvent</td>
<td>func(*Gui.IWnd, *Gui.CommandStateEvent)</td>
<td></td>
</tr>
<tr>
<td>onApplyStateEvent</td>
<td>func(*Gui.IWnd, *Gui.CommandStateEvent)</td>
<td></td>
</tr>
<tr>
<td>onFrameEvent</td>
<td>func(*Gui.IWnd, *Gui.FrameEvent)</td>
<td></td>
</tr>
<tr>
<td>onSerializeStateEvent</td>
<td>func(*Gui.IWnd, *Gui.SerializeStateEvent)</td>
<td></td>
</tr>
<tr>
<td>onNotifyEvent</td>
<td>func(*Gui.IWnd, *Gui.NotifyEvent)</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Icon"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">Icon</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\icon.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>imageList</td>
<td>*Gui.ImageList</td>
<td></td>
</tr>
<tr>
<td>index</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>sizeX</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>sizeY</td>
<td>f32</td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_Icon_clear">clear(self)</a></td>
<td>Set icon to invalid. </td>
</tr>
<tr>
<td><a href="#Gui_Icon_from">from(*ImageList, s32, f32, f32)</a></td>
<td>Initialize the icon from an image list. </td>
</tr>
<tr>
<td><a href="#Gui_Icon_isValid">isValid(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Icon_paint">paint(self, *Painter, f32, f32, const ref Color)</a></td>
<td>Paint icon at the given position. </td>
</tr>
<tr>
<td><a href="#Gui_Icon_set">set(self, *ImageList, s32, f32, f32)</a></td>
<td>Initialize the icon from an image list. </td>
</tr>
</table>
<h3>Special Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_Icon_opEquals">opEquals(self, const ref Icon)</a></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Icon_clear"><span class="titletype">func</span> <span class="titlelight">Icon.</span><span class="titlestrong">clear</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\icon.swg#L26" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set icon to invalid. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">clear</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Icon_from"><span class="titletype">func</span> <span class="titlelight">Icon.</span><span class="titlestrong">from</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\icon.swg#L32" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Initialize the icon from an image list. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">from</span><span class="SyntaxCode">(imageList: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_ImageList">ImageList</a></span><span class="SyntaxCode">, index: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, sizeX: </span><span class="SyntaxType">f32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">, sizeY: </span><span class="SyntaxType">f32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Icon">Icon</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Icon_isValid"><span class="titletype">func</span> <span class="titlelight">Icon.</span><span class="titlestrong">isValid</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\icon.swg#L23" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">isValid</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Icon_opEquals"><span class="titletype">func</span> <span class="titlelight">Icon.</span><span class="titlestrong">opEquals</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\icon.swg#L14" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">opEquals</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, other: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Icon">Icon</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">bool</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Icon_paint"><span class="titletype">func</span> <span class="titlelight">Icon.</span><span class="titlestrong">paint</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\icon.swg#L53" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Paint icon at the given position. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">paint</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> self, painter: *</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Painter</span><span class="SyntaxCode">, x: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, color: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Color</span><span class="SyntaxCode"> = </span><span class="SyntaxConstant">Argb</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">White</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Icon_set"><span class="titletype">func</span> <span class="titlelight">Icon.</span><span class="titlestrong">set</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\icon.swg#L42" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Initialize the icon from an image list. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">set</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, imageList: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_ImageList">ImageList</a></span><span class="SyntaxCode">, index: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, sizeX: </span><span class="SyntaxType">f32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">, sizeY: </span><span class="SyntaxType">f32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_IconBar"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">IconBar</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\iconbar.swg#L11" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> wnd</td>
<td><a href="#Gui_Wnd">Gui.Wnd</a></td>
<td></td>
</tr>
<tr>
<td>iconBarFlags</td>
<td><a href="#Gui_IconBarFlags">Gui.IconBarFlags</a></td>
<td></td>
</tr>
<tr>
<td>iconSize</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>iconPadding</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>sigUpdateState</td>
<td>Gui.SigArray'(closure(*Gui.CommandStateEvent))</td>
<td></td>
</tr>
<tr>
<td>sigCheckChanged</td>
<td>Gui.SigArray'(closure(*Gui.IconButton))</td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_IconBar_addCheckableItem">addCheckableItem(self, const ref Icon, WndId)</a></td>
<td>Add a new checkable button. </td>
</tr>
<tr>
<td><a href="#Gui_IconBar_addCheckableToggleItem">addCheckableToggleItem(self, const ref Icon, WndId)</a></td>
<td>Add a new checkable button. </td>
</tr>
<tr>
<td><a href="#Gui_IconBar_addItem">addItem(self, const ref Icon, WndId, IconButtonFlags)</a></td>
<td>Add a new button. </td>
</tr>
<tr>
<td><a href="#Gui_IconBar_create">create(*Wnd, f32, const ref Rectangle, WndId)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_IconBar_setIconBarFlags">setIconBarFlags(self, IconBarFlags, IconBarFlags)</a></td>
<td>Change bar flags. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_IconBar_IWnd_onComputeStateEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onComputeStateEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\iconbar.swg#L25" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onComputeStateEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_CommandStateEvent">CommandStateEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_IconBar_addCheckableItem"><span class="titletype">func</span> <span class="titlelight">IconBar.</span><span class="titlestrong">addCheckableItem</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\iconbar.swg#L89" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a new checkable button. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">addCheckableItem</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, icon: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Icon">Icon</a></span><span class="SyntaxCode">, btnId: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_IconButton">IconButton</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_IconBar_addCheckableToggleItem"><span class="titletype">func</span> <span class="titlelight">IconBar.</span><span class="titlestrong">addCheckableToggleItem</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\iconbar.swg#L96" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a new checkable button. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">addCheckableToggleItem</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, icon: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Icon">Icon</a></span><span class="SyntaxCode">, btnId: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_IconButton">IconButton</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_IconBar_addItem"><span class="titletype">func</span> <span class="titlelight">IconBar.</span><span class="titlestrong">addItem</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\iconbar.swg#L65" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a new button. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">addItem</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, icon: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Icon">Icon</a></span><span class="SyntaxCode">, btnId: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">, iconBtnFlags = </span><span class="SyntaxConstant"><a href="#Gui_IconButtonFlags">IconButtonFlags</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Zero</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_IconButton">IconButton</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_IconBar_create"><span class="titletype">func</span> <span class="titlelight">IconBar.</span><span class="titlestrong">create</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\iconbar.swg#L53" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">create</span><span class="SyntaxCode">(parent: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode">, iconSize: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, position: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode"> = {}, id: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_IconBar">IconBar</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_IconBar_setIconBarFlags"><span class="titletype">func</span> <span class="titlelight">IconBar.</span><span class="titlestrong">setIconBarFlags</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\iconbar.swg#L102" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Change bar flags. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setIconBarFlags</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, add = </span><span class="SyntaxConstant"><a href="#Gui_IconBarFlags">IconBarFlags</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Zero</span><span class="SyntaxCode">, remove = </span><span class="SyntaxConstant"><a href="#Gui_IconBarFlags">IconBarFlags</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Zero</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_IconBarFlags"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">IconBarFlags</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\iconbar.swg#L5" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>Zero</td>
<td></td>
</tr>
<tr>
<td>Vertical</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_IconButton"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">IconButton</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\iconbutton.swg#L50" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> button</td>
<td><a href="#Gui_Button">Gui.Button</a></td>
<td></td>
</tr>
<tr>
<td>form</td>
<td><a href="#Gui_IconButtonForm">Gui.IconButtonForm</a></td>
<td></td>
</tr>
<tr>
<td>iconBtnFlags</td>
<td><a href="#Gui_IconButtonFlags">Gui.IconButtonFlags</a></td>
<td></td>
</tr>
<tr>
<td>checkedForm</td>
<td><a href="#Gui_IconButtonCheckedForm">Gui.IconButtonCheckedForm</a></td>
<td></td>
</tr>
<tr>
<td>arrowPos</td>
<td><a href="#Gui_IconButtonArrowPos">Gui.IconButtonArrowPos</a></td>
<td></td>
</tr>
<tr>
<td>textPos</td>
<td><a href="#Gui_IconButtonTextPos">Gui.IconButtonTextPos</a></td>
<td></td>
</tr>
<tr>
<td>sigHidePopup</td>
<td>Gui.SigArray'(closure(*Gui.IconButton))</td>
<td></td>
</tr>
<tr>
<td>sigShowPopup</td>
<td>Gui.SigArray'(closure(*Gui.IconButton))</td>
<td></td>
</tr>
<tr>
<td>sigCheckChanged</td>
<td>Gui.SigArray'(closure(*Gui.IconButton))</td>
<td></td>
</tr>
<tr>
<td>sigPaintIcon</td>
<td>closure(*Gui.IconButton, *Gui.PaintContext, Core.Math.Rectangle)</td>
<td></td>
</tr>
<tr>
<td>aniBk</td>
<td><a href="#Gui_BlendColor">Gui.BlendColor</a></td>
<td></td>
</tr>
<tr>
<td>isChecked</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>popup</td>
<td>*Gui.Wnd</td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_IconButton_create">create(*Wnd, const ref Icon, const ref Rectangle, WndId)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_IconButton_setCheck">setCheck(self, bool)</a></td>
<td>Set the check state of the button (if the button is checkable). </td>
</tr>
<tr>
<td><a href="#Gui_IconButton_setForm">setForm(self, IconButtonForm)</a></td>
<td>Set the button form. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_IconButton_IWnd_hidePopup"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">hidePopup</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\iconbutton.swg#L72" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">hidePopup</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_IconButton_IWnd_onApplyStateEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onApplyStateEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\iconbutton.swg#L105" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onApplyStateEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_CommandStateEvent">CommandStateEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_IconButton_IWnd_onDestroyEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onDestroyEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\iconbutton.swg#L100" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onDestroyEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_DestroyEvent">DestroyEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_IconButton_IWnd_onHookEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onHookEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\iconbutton.swg#L85" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onHookEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Event">Event</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_IconButton_IWnd_onMouseEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onMouseEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\iconbutton.swg#L430" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onMouseEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_MouseEvent">MouseEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_IconButton_IWnd_onPaintEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onPaintEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\iconbutton.swg#L125" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onPaintEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_PaintEvent">PaintEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_IconButton_create"><span class="titletype">func</span> <span class="titlelight">IconButton.</span><span class="titlestrong">create</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\iconbutton.swg#L475" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">create</span><span class="SyntaxCode">(parent: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode">, icon: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Icon">Icon</a></span><span class="SyntaxCode">, position: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode"> = {}, id: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_IconButton">IconButton</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_IconButton_setCheck"><span class="titletype">func</span> <span class="titlelight">IconButton.</span><span class="titlestrong">setCheck</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\iconbutton.swg#L492" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the check state of the button (if the button is checkable). </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setCheck</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, checked: </span><span class="SyntaxType">bool</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_IconButton_setForm"><span class="titletype">func</span> <span class="titlelight">IconButton.</span><span class="titlestrong">setForm</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\iconbutton.swg#L504" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the button form. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setForm</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, form: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_IconButtonForm">IconButtonForm</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_IconButtonArrowPos"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">IconButtonArrowPos</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\iconbutton.swg#L21" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>None</td>
<td></td>
</tr>
<tr>
<td>Left</td>
<td></td>
</tr>
<tr>
<td>Top</td>
<td></td>
</tr>
<tr>
<td>Right</td>
<td></td>
</tr>
<tr>
<td>RightUp</td>
<td></td>
</tr>
<tr>
<td>RightDown</td>
<td></td>
</tr>
<tr>
<td>Bottom</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_IconButtonCheckedForm"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">IconButtonCheckedForm</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\iconbutton.swg#L40" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>None</td>
<td></td>
</tr>
<tr>
<td>Bottom</td>
<td></td>
</tr>
<tr>
<td>Top</td>
<td></td>
</tr>
<tr>
<td>Left</td>
<td></td>
</tr>
<tr>
<td>Right</td>
<td></td>
</tr>
<tr>
<td>Full</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_IconButtonFlags"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">IconButtonFlags</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\iconbutton.swg#L12" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>Zero</td>
<td></td>
</tr>
<tr>
<td>Checkable</td>
<td></td>
</tr>
<tr>
<td>CheckableToggle</td>
<td></td>
</tr>
<tr>
<td>Popup</td>
<td></td>
</tr>
<tr>
<td>Center</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_IconButtonForm"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">IconButtonForm</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\iconbutton.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>Square</td>
<td></td>
</tr>
<tr>
<td>Round</td>
<td></td>
</tr>
<tr>
<td>RoundSquare</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_IconButtonTextPos"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">IconButtonTextPos</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\iconbutton.swg#L32" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>Left</td>
<td></td>
</tr>
<tr>
<td>Top</td>
<td></td>
</tr>
<tr>
<td>Right</td>
<td></td>
</tr>
<tr>
<td>Bottom</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ImageList"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">ImageList</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\imagelist.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>texture</td>
<td>Pixel.Texture</td>
<td></td>
</tr>
<tr>
<td>totalFrames</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>frameSizeX</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>frameSizeY</td>
<td>s32</td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_ImageList_countX">countX(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_ImageList_countY">countY(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_ImageList_set">set(self, const ref Texture, s32, s32, s32)</a></td>
<td>Initialize image list. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ImageList_countX"><span class="titletype">func</span> <span class="titlelight">ImageList.</span><span class="titlestrong">countX</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\imagelist.swg#L16" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">countX</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ImageList_countY"><span class="titletype">func</span> <span class="titlelight">ImageList.</span><span class="titlestrong">countY</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\imagelist.swg#L17" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">countY</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ImageList_set"><span class="titletype">func</span> <span class="titlelight">ImageList.</span><span class="titlestrong">set</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\imagelist.swg#L21" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Initialize image list. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">set</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, texture: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Texture</span><span class="SyntaxCode">, fsx: </span><span class="SyntaxType">s32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">, fsy: </span><span class="SyntaxType">s32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">, cpt: </span><span class="SyntaxType">s32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ImageRect"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">ImageRect</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\imagerect.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> wnd</td>
<td><a href="#Gui_Wnd">Gui.Wnd</a></td>
<td></td>
</tr>
<tr>
<td>movie</td>
<td><a href="#Gui_Movie">Gui.Movie</a></td>
<td></td>
</tr>
<tr>
<td>textureFrame</td>
<td>Pixel.Texture</td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_ImageRect_clear">clear(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_ImageRect_create">create(*Wnd, const ref Rectangle, WndId)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_ImageRect_setFile">setFile(self, string)</a></td>
<td>Load and set movie. </td>
</tr>
<tr>
<td><a href="#Gui_ImageRect_setImage">setImage(self, const ref ImageList)</a></td>
<td>Associate image. </td>
</tr>
<tr>
<td><a href="#Gui_ImageRect_setImage">setImage(self, const ref Image)</a></td>
<td>Associate image. </td>
</tr>
<tr>
<td><a href="#Gui_ImageRect_setImage">setImage(self, const ref Texture)</a></td>
<td>Associate image. </td>
</tr>
<tr>
<td><a href="#Gui_ImageRect_setMovie">setMovie(self, const ref ImageList)</a></td>
<td>Set movie as an image list. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ImageRect_IWnd_onPaintEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onPaintEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\imagerect.swg#L13" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onPaintEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_PaintEvent">PaintEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ImageRect_clear"><span class="titletype">func</span> <span class="titlelight">ImageRect.</span><span class="titlestrong">clear</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\imagerect.swg#L59" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">clear</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ImageRect_create"><span class="titletype">func</span> <span class="titlelight">ImageRect.</span><span class="titlestrong">create</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\imagerect.swg#L52" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">create</span><span class="SyntaxCode">(parent: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode">, position: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode"> = {}, id: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_ImageRect">ImageRect</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ImageRect_setFile"><span class="titletype">func</span> <span class="titlelight">ImageRect.</span><span class="titlestrong">setFile</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\imagerect.swg#L108" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Load and set movie. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setFile</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, fileName: </span><span class="SyntaxType">string</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ImageRect_setImage"><span class="titletype">func</span> <span class="titlelight">ImageRect.</span><span class="titlestrong">setImage</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\imagerect.swg#L73" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Associate image. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setImage</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, image: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_ImageList">ImageList</a></span><span class="SyntaxCode">)</code>
</pre>
<p>Associate image. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setImage</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, image: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Image</span><span class="SyntaxCode">)</code>
</pre>
<p>Associate image. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setImage</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, image: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Texture</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ImageRect_setMovie"><span class="titletype">func</span> <span class="titlelight">ImageRect.</span><span class="titlestrong">setMovie</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\imagerect.swg#L65" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set movie as an image list. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setMovie</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, image: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_ImageList">ImageList</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Item"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">Item</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\item.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>name</td>
<td>Pixel.RichString</td>
<td></td>
</tr>
<tr>
<td>icon</td>
<td><a href="#Gui_Icon">Gui.Icon</a></td>
<td></td>
</tr>
<tr>
<td>id</td>
<td>Gui.WndId</td>
<td></td>
</tr>
<tr>
<td>userData0</td>
<td>*void</td>
<td></td>
</tr>
<tr>
<td>userData1</td>
<td>*void</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_KeyEvent"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">KeyEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\events.swg#L152" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> base</td>
<td><a href="#Gui_Event">Gui.Event</a></td>
<td></td>
</tr>
<tr>
<td>modifiers</td>
<td>Core.Input.KeyModifiers</td>
<td></td>
</tr>
<tr>
<td>key</td>
<td>Core.Input.Key</td>
<td></td>
</tr>
<tr>
<td>char</td>
<td>rune</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_KeyShortcut"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">KeyShortcut</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L42" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>mdf</td>
<td>Core.Input.KeyModifiers</td>
<td></td>
</tr>
<tr>
<td>key</td>
<td>Core.Input.Key</td>
<td></td>
</tr>
<tr>
<td>id</td>
<td>Gui.WndId</td>
<td></td>
</tr>
<tr>
<td>target</td>
<td>*Gui.Wnd</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Label"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">Label</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\label.swg#L13" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> wnd</td>
<td><a href="#Gui_Wnd">Gui.Wnd</a></td>
<td></td>
</tr>
<tr>
<td>icon</td>
<td><a href="#Gui_Icon">Gui.Icon</a></td>
<td></td>
</tr>
<tr>
<td>iconColor</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>labelFlags</td>
<td><a href="#Gui_LabelFlags">Gui.LabelFlags</a></td>
<td></td>
</tr>
<tr>
<td>horzAlignmentIcon</td>
<td>Pixel.StringHorzAlignment</td>
<td></td>
</tr>
<tr>
<td>vertAlignmentIcon</td>
<td>Pixel.StringVertAlignment</td>
<td></td>
</tr>
<tr>
<td>horzAlignmentText</td>
<td>Pixel.StringHorzAlignment</td>
<td></td>
</tr>
<tr>
<td>vertAlignmentText</td>
<td>Pixel.StringVertAlignment</td>
<td></td>
</tr>
<tr>
<td>paddingText</td>
<td>Core.Math.Vector4</td>
<td></td>
</tr>
<tr>
<td>paddingIcon</td>
<td>Core.Math.Vector4</td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_Label_create">create(*Wnd, string, const ref Rectangle, WndId)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Label_getText">getText(self)</a></td>
<td>Get the text. </td>
</tr>
<tr>
<td><a href="#Gui_Label_setText">setText(self, string)</a></td>
<td>Change the text. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Label_IWnd_onPaintEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onPaintEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\label.swg#L30" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onPaintEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_PaintEvent">PaintEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Label_create"><span class="titletype">func</span> <span class="titlelight">Label.</span><span class="titlestrong">create</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\label.swg#L117" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">create</span><span class="SyntaxCode">(parent: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode">, name: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, position: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode"> = {}, id: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Label">Label</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Label_getText"><span class="titletype">func</span> <span class="titlelight">Label.</span><span class="titlestrong">getText</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\label.swg#L149" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the text. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getText</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxType">string</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Label_setText"><span class="titletype">func</span> <span class="titlelight">Label.</span><span class="titlestrong">setText</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\label.swg#L141" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Change the text. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setText</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, text: </span><span class="SyntaxType">string</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_LabelFlags"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">LabelFlags</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\label.swg#L5" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>Zero</td>
<td></td>
</tr>
<tr>
<td>WordWrap</td>
<td></td>
</tr>
<tr>
<td>LightText</td>
<td></td>
</tr>
<tr>
<td>AutoHeight</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ListCtrl"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">ListCtrl</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L56" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> frameWnd</td>
<td><a href="#Gui_FrameWnd">Gui.FrameWnd</a></td>
<td></td>
</tr>
<tr>
<td>horizontalExtent</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>lineHeight</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>indentWidth</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>iconSize</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>iconMarginFirstCol</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>listFlags</td>
<td><a href="#Gui_ListFlags">Gui.ListFlags</a></td>
<td></td>
</tr>
<tr>
<td>selectionMode</td>
<td><a href="#Gui_ListSelectionMode">Gui.ListSelectionMode</a></td>
<td></td>
</tr>
<tr>
<td>leftTextMargin</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>sigSelChanged</td>
<td>Gui.SigArray'(closure(*Gui.ListCtrl))</td>
<td></td>
</tr>
<tr>
<td>sigCheckChanged</td>
<td>Gui.SigArray'(closure(*Gui.ListCtrl))</td>
<td></td>
</tr>
<tr>
<td>sigRightClick</td>
<td>Gui.SigArray'(closure(*Gui.ListCtrl, Core.Math.Point))</td>
<td></td>
</tr>
<tr>
<td>sigLeftDoubleClick</td>
<td>Gui.SigArray'(closure(*Gui.ListCtrl, Core.Math.Point))</td>
<td></td>
</tr>
<tr>
<td>sigExpand</td>
<td>Gui.SigArray'(closure(*Gui.ListCtrl, *Gui.ListLine))</td>
<td></td>
</tr>
<tr>
<td>sigCollapse</td>
<td>Gui.SigArray'(closure(*Gui.ListCtrl, *Gui.ListLine))</td>
<td></td>
</tr>
<tr>
<td>sigKeyPressed</td>
<td>Gui.SigArray'(closure(*Gui.ListCtrl, *Gui.KeyEvent))</td>
<td></td>
</tr>
<tr>
<td>sigVirtualFill</td>
<td>func(*Gui.ListCtrl, *Gui.ListLine, u32)</td>
<td></td>
</tr>
<tr>
<td>sigSort</td>
<td>func(*Gui.ListLine, *Gui.ListLine, u32, bool)->s32</td>
<td></td>
</tr>
<tr>
<td>toFreeLines</td>
<td>Core.ArrayPtr'(Gui.ListLine)</td>
<td></td>
</tr>
<tr>
<td>lines</td>
<td>Core.Array'(*Gui.ListLine)</td>
<td></td>
</tr>
<tr>
<td>idxLineToIdxList</td>
<td>Core.HashTable'(u32, u32)</td>
<td></td>
</tr>
<tr>
<td>selModel</td>
<td><a href="#Gui_SelModel">Gui.SelModel</a></td>
<td></td>
</tr>
<tr>
<td>checkModel</td>
<td><a href="#Gui_SelModel">Gui.SelModel</a></td>
<td></td>
</tr>
<tr>
<td>focusIndex</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>hotIndex</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>scrollWnd</td>
<td>*Gui.ScrollWnd</td>
<td></td>
</tr>
<tr>
<td>listView</td>
<td>*Gui.ListView</td>
<td></td>
</tr>
<tr>
<td>header</td>
<td>*Gui.Header</td>
<td></td>
</tr>
<tr>
<td>virtualCount</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>isVirtual</td>
<td>bool</td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_ListCtrl_addColumn">addColumn(self, string, f32, const ref Icon)</a></td>
<td>Add one column in a multi columns list. </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_clear">clear(self)</a></td>
<td>Clear all lines. </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_collapse">collapse(self, *ListLine)</a></td>
<td>Collapse the given line. </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_collapseAll">collapseAll(self)</a></td>
<td>Collapse all lines. </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_computeLayout">computeLayout(self)</a></td>
<td>Compute global layout, once all lines have been added. </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_createLine">createLine(self, *ListLine)</a></td>
<td>Add one new line. </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_createLine">createLine(self, string, const ref Icon, *ListLine)</a></td>
<td>Add one line, and set first column. </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_createMultiColumns">createMultiColumns(*Wnd, const ref Rectangle, WndId)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_createSimple">createSimple(*Wnd, const ref Rectangle, WndId)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_ensureVisibleLine">ensureVisibleLine(self, u32)</a></td>
<td>Set the corresponding line visible. </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_expand">expand(self, *ListLine)</a></td>
<td>Expand the given line. </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_getCheckedLines">getCheckedLines(self)</a></td>
<td>Get all the checked lines. </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_getFirstVisibleLineIndex">getFirstVisibleLineIndex(self)</a></td>
<td>Returns the first visible line index. </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_getFocusLine">getFocusLine(self)</a></td>
<td>Get the line with the keyboard focus. </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_getLastVisibleLineIndex">getLastVisibleLineIndex(self)</a></td>
<td>Returns the last visible line index. </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_getLine">getLine(self, u32)</a></td>
<td>Get a line by index. </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_getLineCount">getLineCount(self)</a></td>
<td>Returns the number of lines in the list. </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_getLineHeight">getLineHeight(self)</a></td>
<td>Get the height of one line. </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_getSelectedLine">getSelectedLine(self)</a></td>
<td>Returns a selected line  Mostly for singlesel mode. </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_getSelectedLines">getSelectedLines(self)</a></td>
<td>Get all the selected lines. </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_isLineSelected">isLineSelected(self, u32)</a></td>
<td>Returns. </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_isSelectedLine">isSelectedLine(self, u32)</a></td>
<td>Returns true if the line at the given index is selected. </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_selectLine">selectLine(self, u32, bool)</a></td>
<td>Select of unselect the given line. </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_selectLines">selectLines(self, u32, u32, bool)</a></td>
<td>Select a range of lines. </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_setColumnWidth">setColumnWidth(self, u32, f32)</a></td>
<td>Set the column width. </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_setFocus">setFocus(self)</a></td>
<td>Set focus to the list. </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_setVirtualCount">setVirtualCount(self, u32)</a></td>
<td>Set the number of lines in a virtual list. </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_sort">sort(self)</a></td>
<td>Sort list as before. </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_sort">sort(self, u32, bool, bool)</a></td>
<td>Sort the list by a given column  If <code class="incode">persistent</code> is true, then the sorted column will be displayed in the  header (if it exists). </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_toggleExpand">toggleExpand(self, *ListLine)</a></td>
<td>Collapse the given line. </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_unselectAll">unselectAll(self)</a></td>
<td>Unselect all lines. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ListCtrl_addColumn"><span class="titletype">func</span> <span class="titlelight">ListCtrl.</span><span class="titlestrong">addColumn</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L999" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add one column in a multi columns list. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">addColumn</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, name: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, width: </span><span class="SyntaxType">f32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">100</span><span class="SyntaxCode">, icon: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Icon">Icon</a></span><span class="SyntaxCode"> = {})</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ListCtrl_clear"><span class="titletype">func</span> <span class="titlelight">ListCtrl.</span><span class="titlestrong">clear</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L988" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Clear all lines. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">clear</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ListCtrl_collapse"><span class="titletype">func</span> <span class="titlelight">ListCtrl.</span><span class="titlestrong">collapse</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L1240" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Collapse the given line. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">collapse</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, line: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_ListLine">ListLine</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ListCtrl_collapseAll"><span class="titletype">func</span> <span class="titlelight">ListCtrl.</span><span class="titlestrong">collapseAll</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L1282" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Collapse all lines. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">collapseAll</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ListCtrl_computeLayout"><span class="titletype">func</span> <span class="titlelight">ListCtrl.</span><span class="titlestrong">computeLayout</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L920" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Compute global layout, once all lines have been added. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">computeLayout</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ListCtrl_createLine"><span class="titletype">func</span> <span class="titlelight">ListCtrl.</span><span class="titlestrong">createLine</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L957" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add one new line. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">createLine</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, parentLine: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_ListLine">ListLine</a></span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_ListLine">ListLine</a></span><span class="SyntaxCode"></code>
</pre>
<p>Add one line, and set first column. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">createLine</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, name: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, icon: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Icon">Icon</a></span><span class="SyntaxCode"> = {}, parentLine: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_ListLine">ListLine</a></span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_ListLine">ListLine</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ListCtrl_createMultiColumns"><span class="titletype">func</span> <span class="titlelight">ListCtrl.</span><span class="titlestrong">createMultiColumns</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L787" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">createMultiColumns</span><span class="SyntaxCode">(parent: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode">, position: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode"> = {}, id: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_ListCtrl">ListCtrl</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ListCtrl_createSimple"><span class="titletype">func</span> <span class="titlelight">ListCtrl.</span><span class="titlestrong">createSimple</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L769" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">createSimple</span><span class="SyntaxCode">(parent: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode">, position: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode"> = {}, id: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_ListCtrl">ListCtrl</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ListCtrl_ensureVisibleLine"><span class="titletype">func</span> <span class="titlelight">ListCtrl.</span><span class="titlestrong">ensureVisibleLine</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L1050" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the corresponding line visible. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">ensureVisibleLine</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, idx: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ListCtrl_expand"><span class="titletype">func</span> <span class="titlelight">ListCtrl.</span><span class="titlestrong">expand</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L1198" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Expand the given line. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">expand</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, line: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_ListLine">ListLine</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ListCtrl_getCheckedLines"><span class="titletype">func</span> <span class="titlelight">ListCtrl.</span><span class="titlestrong">getCheckedLines</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L1092" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get all the checked lines. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getCheckedLines</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Array</span><span class="SyntaxCode">'(*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_ListLine">ListLine</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ListCtrl_getFirstVisibleLineIndex"><span class="titletype">func</span> <span class="titlelight">ListCtrl.</span><span class="titlestrong">getFirstVisibleLineIndex</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L1022" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the first visible line index. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getFirstVisibleLineIndex</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxType">u32</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ListCtrl_getFocusLine"><span class="titletype">func</span> <span class="titlelight">ListCtrl.</span><span class="titlestrong">getFocusLine</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L1068" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the line with the keyboard focus. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getFocusLine</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> self)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_ListLine">ListLine</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ListCtrl_getLastVisibleLineIndex"><span class="titletype">func</span> <span class="titlelight">ListCtrl.</span><span class="titlestrong">getLastVisibleLineIndex</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L1032" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the last visible line index. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getLastVisibleLineIndex</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxType">u32</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ListCtrl_getLine"><span class="titletype">func</span> <span class="titlelight">ListCtrl.</span><span class="titlestrong">getLine</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L943" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get a line by index. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getLine</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, idx: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_ListLine">ListLine</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ListCtrl_getLineCount"><span class="titletype">func</span> <span class="titlelight">ListCtrl.</span><span class="titlestrong">getLineCount</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L949" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the number of lines in the list. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getLineCount</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxType">u32</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ListCtrl_getLineHeight"><span class="titletype">func</span> <span class="titlelight">ListCtrl.</span><span class="titlestrong">getLineHeight</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L937" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the height of one line. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getLineHeight</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ListCtrl_getSelectedLine"><span class="titletype">func</span> <span class="titlelight">ListCtrl.</span><span class="titlestrong">getSelectedLine</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L1109" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns a selected line  Mostly for singlesel mode. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getSelectedLine</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> self)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_ListLine">ListLine</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ListCtrl_getSelectedLines"><span class="titletype">func</span> <span class="titlelight">ListCtrl.</span><span class="titlestrong">getSelectedLines</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L1076" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get all the selected lines. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getSelectedLines</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Array</span><span class="SyntaxCode">'(*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_ListLine">ListLine</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ListCtrl_isLineSelected"><span class="titletype">func</span> <span class="titlelight">ListCtrl.</span><span class="titlestrong">isLineSelected</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L867" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">isLineSelected</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, idx: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">bool</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ListCtrl_isSelectedLine"><span class="titletype">func</span> <span class="titlelight">ListCtrl.</span><span class="titlestrong">isSelectedLine</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L1061" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns true if the line at the given index is selected. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">isSelectedLine</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> self, idx: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">bool</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ListCtrl_selectLine"><span class="titletype">func</span> <span class="titlelight">ListCtrl.</span><span class="titlestrong">selectLine</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L878" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Select of unselect the given line. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">selectLine</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, idx: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">, select: </span><span class="SyntaxType">bool</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ListCtrl_selectLines"><span class="titletype">func</span> <span class="titlelight">ListCtrl.</span><span class="titlestrong">selectLines</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L899" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Select a range of lines. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">selectLines</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, start: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">, end: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">, select: </span><span class="SyntaxType">bool</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ListCtrl_setColumnWidth"><span class="titletype">func</span> <span class="titlelight">ListCtrl.</span><span class="titlestrong">setColumnWidth</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L1006" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the column width. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setColumnWidth</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, idx: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">, width: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ListCtrl_setFocus"><span class="titletype">func</span> <span class="titlelight">ListCtrl.</span><span class="titlestrong">setFocus</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L1313" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set focus to the list. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setFocus</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ListCtrl_setVirtualCount"><span class="titletype">func</span> <span class="titlelight">ListCtrl.</span><span class="titlestrong">setVirtualCount</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L1013" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the number of lines in a virtual list. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setVirtualCount</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, count: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ListCtrl_sort"><span class="titletype">func</span> <span class="titlelight">ListCtrl.</span><span class="titlestrong">sort</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L1189" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Sort list as before. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">sort</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>Sort the list by a given column  If <code class="incode">persistent</code> is true, then the sorted column will be displayed in the  header (if it exists). </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">sort</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, column: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">, descentOrder = </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">, persistent = </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ListCtrl_toggleExpand"><span class="titletype">func</span> <span class="titlelight">ListCtrl.</span><span class="titlestrong">toggleExpand</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L1304" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Collapse the given line. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">toggleExpand</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, line: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_ListLine">ListLine</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ListCtrl_unselectAll"><span class="titletype">func</span> <span class="titlelight">ListCtrl.</span><span class="titlestrong">unselectAll</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L857" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Unselect all lines. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">unselectAll</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ListFlags"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">ListFlags</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L12" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>Zero</td>
<td></td>
</tr>
<tr>
<td>HideSelection</td>
<td></td>
</tr>
<tr>
<td>AlternateLines</td>
<td></td>
</tr>
<tr>
<td>HorzLines</td>
<td></td>
</tr>
<tr>
<td>VertLines</td>
<td></td>
</tr>
<tr>
<td>ForceMarginExpandMark</td>
<td>Force one margin even if no expand mark. </td>
</tr>
<tr>
<td>NoMouseEmptySel</td>
<td>Click does not clear selection if not on an item. </td>
</tr>
<tr>
<td>HotTrack</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ListItem"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">ListItem</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L24" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>sortKey</td>
<td>Core.String</td>
<td></td>
</tr>
<tr>
<td>colorBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>colorFg</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>column</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>horzAlign</td>
<td>Pixel.StringHorzAlignment</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ListLine"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">ListLine</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L34" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>colorBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>colorFg</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>userData0</td>
<td>*void</td>
<td></td>
</tr>
<tr>
<td>userData1</td>
<td>*void</td>
<td></td>
</tr>
<tr>
<td>userData2</td>
<td>*void</td>
<td></td>
</tr>
<tr>
<td>userData3</td>
<td>*void</td>
<td></td>
</tr>
<tr>
<td>leftTextMargin</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>forceExpandMark</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>canCheck</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>separator</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>iconMargin</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>lines</td>
<td>Core.ArrayPtr'(Gui.ListLine)</td>
<td></td>
</tr>
<tr>
<td>parent</td>
<td>*Gui.ListLine</td>
<td></td>
</tr>
<tr>
<td>index</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>level</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>expanded</td>
<td>bool</td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_ListLine_canExpand">canExpand(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_ListLine_getItem">getItem(self, u32)</a></td>
<td>Get an item by index. </td>
</tr>
<tr>
<td><a href="#Gui_ListLine_isParentOf">isParentOf(self, *ListLine)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_ListLine_setItem">setItem(self, u32, string, const ref Icon)</a></td>
<td>Initialize a given column item. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ListLine_canExpand"><span class="titletype">func</span> <span class="titlelight">ListLine.</span><span class="titlestrong">canExpand</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L111" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">canExpand</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxType">bool</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ListLine_getItem"><span class="titletype">func</span> <span class="titlelight">ListLine.</span><span class="titlestrong">getItem</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L117" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get an item by index. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getItem</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, column: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_ListItem">ListItem</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ListLine_isParentOf"><span class="titletype">func</span> <span class="titlelight">ListLine.</span><span class="titlestrong">isParentOf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L102" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">isParentOf</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, other: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_ListLine">ListLine</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">bool</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ListLine_setItem"><span class="titletype">func</span> <span class="titlelight">ListLine.</span><span class="titlestrong">setItem</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L130" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Initialize a given column item. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setItem</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, column: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">, name: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, icon: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Icon">Icon</a></span><span class="SyntaxCode"> = {})-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_ListItem">ListItem</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ListSelectionMode"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">ListSelectionMode</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>None</td>
<td></td>
</tr>
<tr>
<td>Single</td>
<td></td>
</tr>
<tr>
<td>Multi</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ListView"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">ListView</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L93" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> wnd</td>
<td><a href="#Gui_Wnd">Gui.Wnd</a></td>
<td></td>
</tr>
<tr>
<td>list</td>
<td>*Gui.ListCtrl</td>
<td></td>
</tr>
<tr>
<td>discardMouseRelease</td>
<td>bool</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ListView_IWnd_onKeyEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onKeyEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L159" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onKeyEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_KeyEvent">KeyEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ListView_IWnd_onMouseEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onMouseEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L304" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onMouseEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_MouseEvent">MouseEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ListView_IWnd_onPaintEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onPaintEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L490" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onPaintEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_PaintEvent">PaintEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ListView_IWnd_onResizeEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onResizeEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L153" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onResizeEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_ResizeEvent">ResizeEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_MenuCtrl"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">MenuCtrl</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\menuctrl.swg#L34" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> frameWnd</td>
<td><a href="#Gui_FrameWnd">Gui.FrameWnd</a></td>
<td></td>
</tr>
<tr>
<td>onValidateResult</td>
<td>func(*Gui.MenuCtrl, Gui.WndId)</td>
<td></td>
</tr>
<tr>
<td>onComputeItem</td>
<td>func(*Gui.MenuCtrl, *Gui.PopupMenuItem)</td>
<td></td>
</tr>
<tr>
<td>menuCtrlFlags</td>
<td><a href="#Gui_MenuCtrlFlags">Gui.MenuCtrlFlags</a></td>
<td></td>
</tr>
<tr>
<td>paddingSel</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>hotIdx</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>selectedIdx</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>bar</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>endModal</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>endModalResult</td>
<td>Gui.WndId</td>
<td></td>
</tr>
<tr>
<td>ownerMenu</td>
<td>*Gui.MenuCtrl</td>
<td></td>
</tr>
<tr>
<td>subVisible</td>
<td>*Gui.MenuCtrl</td>
<td></td>
</tr>
<tr>
<td>timer</td>
<td>*Gui.Timer</td>
<td></td>
</tr>
<tr>
<td>dirtyLayout</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>paintByFrame</td>
<td>bool</td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_MenuCtrl_addItem">addItem(self, WndId)</a></td>
<td>Add a new item. </td>
</tr>
<tr>
<td><a href="#Gui_MenuCtrl_addItem">addItem(self, string, WndId, const ref Icon, string, bool, bool)</a></td>
<td>Add a new item. </td>
</tr>
<tr>
<td><a href="#Gui_MenuCtrl_addPopup">addPopup(self, *MenuCtrl, WndId)</a></td>
<td>Add a new popup item. </td>
</tr>
<tr>
<td><a href="#Gui_MenuCtrl_addPopup">addPopup(self, string, *MenuCtrl, const ref Icon, WndId)</a></td>
<td>Add a new popup item. </td>
</tr>
<tr>
<td><a href="#Gui_MenuCtrl_addSeparator">addSeparator(self)</a></td>
<td>Add a separator. </td>
</tr>
<tr>
<td><a href="#Gui_MenuCtrl_computeLayoutPopup">computeLayoutPopup(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_MenuCtrl_createBar">createBar(*Wnd, const ref Rectangle, WndId, *Wnd)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_MenuCtrl_createPopup">createPopup(*Wnd, WndId, *Wnd)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_MenuCtrl_doModal">doModal(self, const ref Point, PopupPos, bool)</a></td>
<td>Make the popup menu modal  Will return the selected id. </td>
</tr>
<tr>
<td><a href="#Gui_MenuCtrl_updateState">updateState(self)</a></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_MenuCtrl_IWnd_onMouseEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onMouseEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\menuctrl.swg#L267" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onMouseEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_MouseEvent">MouseEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_MenuCtrl_IWnd_onPaintEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onPaintEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\menuctrl.swg#L251" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onPaintEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_PaintEvent">PaintEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_MenuCtrl_IWnd_onTimerEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onTimerEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\menuctrl.swg#L260" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onTimerEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_TimerEvent">TimerEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_MenuCtrl_addItem"><span class="titletype">func</span> <span class="titlelight">MenuCtrl.</span><span class="titlestrong">addItem</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\menuctrl.swg#L666" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a new item. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">addItem</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, id: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode">)</code>
</pre>
<p>Add a new item. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">addItem</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, name: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, id: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode">, icon: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Icon">Icon</a></span><span class="SyntaxCode"> = {}, rightName: </span><span class="SyntaxType">string</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">, disabled = </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">, checked = </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_MenuCtrl_addPopup"><span class="titletype">func</span> <span class="titlelight">MenuCtrl.</span><span class="titlestrong">addPopup</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\menuctrl.swg#L689" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a new popup item. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">addPopup</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, popup: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_MenuCtrl">MenuCtrl</a></span><span class="SyntaxCode">, id: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode">)</code>
</pre>
<p>Add a new popup item. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">addPopup</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, name: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, popup: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_MenuCtrl">MenuCtrl</a></span><span class="SyntaxCode">, icon: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Icon">Icon</a></span><span class="SyntaxCode"> = {}, id: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_MenuCtrl_addSeparator"><span class="titletype">func</span> <span class="titlelight">MenuCtrl.</span><span class="titlestrong">addSeparator</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\menuctrl.swg#L711" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a separator. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">addSeparator</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_MenuCtrl_computeLayoutPopup"><span class="titletype">func</span> <span class="titlelight">MenuCtrl.</span><span class="titlestrong">computeLayoutPopup</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\menuctrl.swg#L566" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">computeLayoutPopup</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_MenuCtrl_createBar"><span class="titletype">func</span> <span class="titlelight">MenuCtrl.</span><span class="titlestrong">createBar</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\menuctrl.swg#L650" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">createBar</span><span class="SyntaxCode">(parent: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode">, pos: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode"> = {}, id: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">, owner: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_MenuCtrl">MenuCtrl</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_MenuCtrl_createPopup"><span class="titletype">func</span> <span class="titlelight">MenuCtrl.</span><span class="titlestrong">createPopup</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\menuctrl.swg#L637" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">createPopup</span><span class="SyntaxCode">(parent: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode">, id: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">, owner: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_MenuCtrl">MenuCtrl</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_MenuCtrl_doModal"><span class="titletype">func</span> <span class="titlelight">MenuCtrl.</span><span class="titlestrong">doModal</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\menuctrl.swg#L721" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Make the popup menu modal  Will return the selected id. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">doModal</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, surfacePos: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Point</span><span class="SyntaxCode">, pos = </span><span class="SyntaxConstant"><a href="#Gui_PopupPos">PopupPos</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant">TopLeft</span><span class="SyntaxCode">, autoDestroy: </span><span class="SyntaxType">bool</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_MenuCtrl_updateState"><span class="titletype">func</span> <span class="titlelight">MenuCtrl.</span><span class="titlestrong">updateState</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\menuctrl.swg#L395" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">updateState</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_MenuCtrlFlags"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">MenuCtrlFlags</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\menuctrl.swg#L11" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>Zero</td>
<td></td>
</tr>
<tr>
<td>NoKeyShortcuts</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_MessageDlg"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">MessageDlg</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\messagedlg.swg#L3" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> dialog</td>
<td><a href="#Gui_Dialog">Gui.Dialog</a></td>
<td></td>
</tr>
<tr>
<td>labelIcon</td>
<td>*Gui.Label</td>
<td></td>
</tr>
<tr>
<td>label</td>
<td>*Gui.Label</td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_MessageDlg_confirm">confirm(*Surface, string, string)</a></td>
<td>Message box to <code class="incode">confirm</code> something. </td>
</tr>
<tr>
<td><a href="#Gui_MessageDlg_create">create(*Surface, string, const ref Icon)</a></td>
<td>Creates the message box. </td>
</tr>
<tr>
<td><a href="#Gui_MessageDlg_doModal">doModal(self, *Surface)</a></td>
<td>Display the box, centered, and returns the id of the pressed button. </td>
</tr>
<tr>
<td><a href="#Gui_MessageDlg_error">error(*Surface, string, string)</a></td>
<td>Message box to show an error. </td>
</tr>
<tr>
<td><a href="#Gui_MessageDlg_ok">ok(*Surface, string, const ref Icon)</a></td>
<td>Message box with a <code class="incode">ok</code>. </td>
</tr>
<tr>
<td><a href="#Gui_MessageDlg_okCancel">okCancel(*Surface, string, string, const ref Icon)</a></td>
<td>Message box with a <code class="incode">ok</code> and <code class="incode">cancel</code> button. </td>
</tr>
<tr>
<td><a href="#Gui_MessageDlg_setIcon">setIcon(self, const ref Icon, const ref Color)</a></td>
<td>Set big icon. </td>
</tr>
<tr>
<td><a href="#Gui_MessageDlg_setIconColor">setIconColor(self, const ref Color)</a></td>
<td>Set big icon color (icon must has been set before). </td>
</tr>
<tr>
<td><a href="#Gui_MessageDlg_yesNo">yesNo(*Surface, string, string, string, const ref Icon)</a></td>
<td>Message box with a <code class="incode">yes</code> and <code class="incode">no</code> button. </td>
</tr>
<tr>
<td><a href="#Gui_MessageDlg_yesNoCancel">yesNoCancel(*Surface, string, string, const ref Icon)</a></td>
<td>Message box with a <code class="incode">yes</code> and <code class="incode">no</code> button. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_MessageDlg_confirm"><span class="titletype">func</span> <span class="titlelight">MessageDlg.</span><span class="titlestrong">confirm</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\messagedlg.swg#L166" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Message box to <code class="incode">confirm</code> something. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">confirm</span><span class="SyntaxCode">(from: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Surface">Surface</a></span><span class="SyntaxCode">, message: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, question: </span><span class="SyntaxType">string</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_MessageDlg_create"><span class="titletype">func</span> <span class="titlelight">MessageDlg.</span><span class="titlestrong">create</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\messagedlg.swg#L13" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Creates the message box. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">create</span><span class="SyntaxCode">(from: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Surface">Surface</a></span><span class="SyntaxCode">, message: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, icon: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Icon">Icon</a></span><span class="SyntaxCode"> = {})-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_MessageDlg">MessageDlg</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_MessageDlg_doModal"><span class="titletype">func</span> <span class="titlelight">MessageDlg.</span><span class="titlestrong">doModal</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\messagedlg.swg#L36" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Display the box, centered, and returns the id of the pressed button. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">doModal</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, from: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Surface">Surface</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">string</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_MessageDlg_error"><span class="titletype">func</span> <span class="titlelight">MessageDlg.</span><span class="titlestrong">error</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\messagedlg.swg#L176" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Message box to show an error. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">error</span><span class="SyntaxCode">(from: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Surface">Surface</a></span><span class="SyntaxCode">, error: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, message: </span><span class="SyntaxType">string</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_MessageDlg_ok"><span class="titletype">func</span> <span class="titlelight">MessageDlg.</span><span class="titlestrong">ok</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\messagedlg.swg#L116" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Message box with a <code class="incode">ok</code>. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">ok</span><span class="SyntaxCode">(from: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Surface">Surface</a></span><span class="SyntaxCode">, message: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, icon: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Icon">Icon</a></span><span class="SyntaxCode"> = {})</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_MessageDlg_okCancel"><span class="titletype">func</span> <span class="titlelight">MessageDlg.</span><span class="titlestrong">okCancel</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\messagedlg.swg#L127" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Message box with a <code class="incode">ok</code> and <code class="incode">cancel</code> button. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">okCancel</span><span class="SyntaxCode">(from: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Surface">Surface</a></span><span class="SyntaxCode">, message: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, defaultId = </span><span class="SyntaxConstant"><a href="#Gui_Dialog">Dialog</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BtnCancel</span><span class="SyntaxCode">, icon: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Icon">Icon</a></span><span class="SyntaxCode"> = {})-&gt;</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_MessageDlg_setIcon"><span class="titletype">func</span> <span class="titlelight">MessageDlg.</span><span class="titlestrong">setIcon</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\messagedlg.swg#L96" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set big icon. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setIcon</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, icon: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Icon">Icon</a></span><span class="SyntaxCode">, color: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Color</span><span class="SyntaxCode"> = </span><span class="SyntaxConstant">Argb</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Zero</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_MessageDlg_setIconColor"><span class="titletype">func</span> <span class="titlelight">MessageDlg.</span><span class="titlestrong">setIconColor</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\messagedlg.swg#L89" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set big icon color (icon must has been set before). </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setIconColor</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, color: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Color</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_MessageDlg_yesNo"><span class="titletype">func</span> <span class="titlelight">MessageDlg.</span><span class="titlestrong">yesNo</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\messagedlg.swg#L139" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Message box with a <code class="incode">yes</code> and <code class="incode">no</code> button. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">yesNo</span><span class="SyntaxCode">(from: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Surface">Surface</a></span><span class="SyntaxCode">, message: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, title: </span><span class="SyntaxType">string</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">, defaultId = </span><span class="SyntaxConstant"><a href="#Gui_Dialog">Dialog</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BtnNo</span><span class="SyntaxCode">, icon: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Icon">Icon</a></span><span class="SyntaxCode"> = {})-&gt;</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_MessageDlg_yesNoCancel"><span class="titletype">func</span> <span class="titlelight">MessageDlg.</span><span class="titlestrong">yesNoCancel</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\messagedlg.swg#L153" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Message box with a <code class="incode">yes</code> and <code class="incode">no</code> button. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">yesNoCancel</span><span class="SyntaxCode">(from: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Surface">Surface</a></span><span class="SyntaxCode">, message: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, defaultId = </span><span class="SyntaxConstant"><a href="#Gui_Dialog">Dialog</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BtnCancel</span><span class="SyntaxCode">, icon: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Icon">Icon</a></span><span class="SyntaxCode"> = {})-&gt;</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_MouseEvent"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">MouseEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\events.swg#L160" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> base</td>
<td><a href="#Gui_Event">Gui.Event</a></td>
<td></td>
</tr>
<tr>
<td>button</td>
<td>Core.Input.MouseButton</td>
<td></td>
</tr>
<tr>
<td>surfacePos</td>
<td>Core.Math.Point</td>
<td></td>
</tr>
<tr>
<td>move</td>
<td>Core.Math.Point</td>
<td></td>
</tr>
<tr>
<td>modifiers</td>
<td>Core.Input.KeyModifiers</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_MoveEvent"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">MoveEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\events.swg#L146" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> base</td>
<td><a href="#Gui_Event">Gui.Event</a></td>
<td></td>
</tr>
<tr>
<td>oldPos</td>
<td>Core.Math.Point</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Movie"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">Movie</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\movie.swg#L11" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>enableCache</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>mustLoop</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>inPause</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>playFreq</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>sigFrameChanged</td>
<td>Gui.SigArray'(closure(*Gui.Movie))</td>
<td></td>
</tr>
<tr>
<td>numFrames</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>frameIndex</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>frame</td>
<td>Pixel.Image</td>
<td></td>
</tr>
<tr>
<td>source</td>
<td><a href="#Gui_MovieSource">Gui.MovieSource</a></td>
<td></td>
</tr>
<tr>
<td>imgList</td>
<td><a href="#Gui_ImageList">Gui.ImageList</a></td>
<td></td>
</tr>
<tr>
<td>gif</td>
<td>Pixel.Gif.Decoder</td>
<td></td>
</tr>
<tr>
<td>bytes</td>
<td>Core.Array'(u8)</td>
<td></td>
</tr>
<tr>
<td>cacheFrames</td>
<td>Core.Array'(Pixel.Image)</td>
<td></td>
</tr>
<tr>
<td>currentTime</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>dirtyFrame</td>
<td>bool</td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_Movie_clear">clear(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Movie_isValid">isValid(self)</a></td>
<td>Returns true if the movie has valid content. </td>
</tr>
<tr>
<td><a href="#Gui_Movie_pause">pause(self, bool)</a></td>
<td>Set/Reset pause state. </td>
</tr>
<tr>
<td><a href="#Gui_Movie_set">set(self, const ref ImageList)</a></td>
<td>Associate an image list. </td>
</tr>
<tr>
<td><a href="#Gui_Movie_set">set(self, string)</a></td>
<td>Associate a filename. </td>
</tr>
<tr>
<td><a href="#Gui_Movie_setFrameIndex">setFrameIndex(self, s32)</a></td>
<td>Set the current video frame. </td>
</tr>
<tr>
<td><a href="#Gui_Movie_update">update(self, f32)</a></td>
<td>Update timing and change frames. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Movie_clear"><span class="titletype">func</span> <span class="titlelight">Movie.</span><span class="titlestrong">clear</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\movie.swg#L80" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">clear</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Movie_isValid"><span class="titletype">func</span> <span class="titlelight">Movie.</span><span class="titlestrong">isValid</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\movie.swg#L122" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns true if the movie has valid content. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">isValid</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxType">bool</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Movie_pause"><span class="titletype">func</span> <span class="titlelight">Movie.</span><span class="titlestrong">pause</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\movie.swg#L166" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set/Reset pause state. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">pause</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, state: </span><span class="SyntaxType">bool</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Movie_set"><span class="titletype">func</span> <span class="titlelight">Movie.</span><span class="titlestrong">set</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\movie.swg#L92" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Associate an image list. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">set</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, imageList: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_ImageList">ImageList</a></span><span class="SyntaxCode">)</code>
</pre>
<p>Associate a filename. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">set</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, fileName: </span><span class="SyntaxType">string</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Movie_setFrameIndex"><span class="titletype">func</span> <span class="titlelight">Movie.</span><span class="titlestrong">setFrameIndex</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\movie.swg#L139" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the current video frame. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setFrameIndex</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, idx: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Movie_update"><span class="titletype">func</span> <span class="titlelight">Movie.</span><span class="titlestrong">update</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\movie.swg#L172" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Update timing and change frames. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">update</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, dt: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">bool</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_MovieSource"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">MovieSource</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\movie.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>None</td>
<td></td>
</tr>
<tr>
<td>SpriteSheet</td>
<td></td>
</tr>
<tr>
<td>Gif</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_NativeCursor"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">NativeCursor</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\cursor.win32.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>cursor</td>
<td>Win32.HCURSOR</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_NativeSurface"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">NativeSurface</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.win32.swg#L21" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>hWnd</td>
<td>Win32.HWND</td>
<td></td>
</tr>
<tr>
<td>lastDeadChar</td>
<td>Win32.WPARAM</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_NotifyEvent"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">NotifyEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\events.swg#L218" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> base</td>
<td><a href="#Gui_Event">Gui.Event</a></td>
<td></td>
</tr>
<tr>
<td>ntfyKind</td>
<td>Gui.NotifyEvent.Kind</td>
<td></td>
</tr>
<tr>
<td>from</td>
<td>*Gui.Wnd</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_PaintContext"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">PaintContext</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\paintcontext.swg#L3" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>painter</td>
<td>*Pixel.Painter</td>
<td></td>
</tr>
<tr>
<td>renderer</td>
<td>*Pixel.RenderOgl</td>
<td></td>
</tr>
<tr>
<td>isDisabled</td>
<td>bool</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_PaintEvent"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">PaintEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\events.swg#L134" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> base</td>
<td><a href="#Gui_Event">Gui.Event</a></td>
<td></td>
</tr>
<tr>
<td>bc</td>
<td>*Gui.PaintContext</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_PaintImage"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">PaintImage</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\paintimage.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_PaintImage_draw">draw(self, func(*void, *Application, *Painter))</a></td>
<td>Draw to an image by calling the closure. </td>
</tr>
<tr>
<td><a href="#Gui_PaintImage_draw">draw(s32, s32, func(*void, *Application, *Painter))</a></td>
<td>One shot paint to image. </td>
</tr>
<tr>
<td><a href="#Gui_PaintImage_init">init(self, s32, s32)</a></td>
<td>First init the PaintImage instance. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_PaintImage_draw"><span class="titletype">func</span> <span class="titlelight">PaintImage.</span><span class="titlestrong">draw</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\paintimage.swg#L59" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Draw to an image by calling the closure. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">draw</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, call: </span><span class="SyntaxKeyword">closure</span><span class="SyntaxCode">(*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Application">Application</a></span><span class="SyntaxCode">, *</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Painter</span><span class="SyntaxCode">))-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Image</span><span class="SyntaxCode"></code>
</pre>
<p>One shot paint to image. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">draw</span><span class="SyntaxCode">(w: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, h: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, call: </span><span class="SyntaxKeyword">closure</span><span class="SyntaxCode">(*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Application">Application</a></span><span class="SyntaxCode">, *</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Painter</span><span class="SyntaxCode">))-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Image</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_PaintImage_init"><span class="titletype">func</span> <span class="titlelight">PaintImage.</span><span class="titlestrong">init</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\paintimage.swg#L50" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>First init the PaintImage instance. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">init</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, w: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, h: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_PalettePicker"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">PalettePicker</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\palettepicker.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> wnd</td>
<td><a href="#Gui_Wnd">Gui.Wnd</a></td>
<td></td>
</tr>
<tr>
<td>selectedColor</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>pal</td>
<td>Core.Array'(Pixel.Color)</td>
<td></td>
</tr>
<tr>
<td>palPos</td>
<td>Core.Array'(Core.Math.Rectangle)</td>
<td></td>
</tr>
<tr>
<td>boxSize</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>boxMargin</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>sigChanged</td>
<td>Gui.SigArray'(closure(*Gui.PalettePicker))</td>
<td></td>
</tr>
<tr>
<td>hotIdx</td>
<td>s32</td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_PalettePicker_addColor">addColor(self, const ref Color)</a></td>
<td>Add a color. </td>
</tr>
<tr>
<td><a href="#Gui_PalettePicker_addHueColors">addHueColors(self, s32, f32, f32)</a></td>
<td>Add a range of changing <code class="incode">hue</code>. </td>
</tr>
<tr>
<td><a href="#Gui_PalettePicker_addLumColors">addLumColors(self, s32, const ref Color)</a></td>
<td>Add a range of changing <code class="incode">luminance</code>. </td>
</tr>
<tr>
<td><a href="#Gui_PalettePicker_addSatColors">addSatColors(self, s32, const ref Color, f32)</a></td>
<td>Add a range of changing <code class="incode">saturation</code>. </td>
</tr>
<tr>
<td><a href="#Gui_PalettePicker_create">create(*Wnd, const ref Rectangle, WndId)</a></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_PalettePicker_IWnd_onMouseEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onMouseEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\palettepicker.swg#L59" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onMouseEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_MouseEvent">MouseEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_PalettePicker_IWnd_onPaintEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onPaintEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\palettepicker.swg#L18" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onPaintEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_PaintEvent">PaintEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_PalettePicker_addColor"><span class="titletype">func</span> <span class="titlelight">PalettePicker.</span><span class="titlestrong">addColor</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\palettepicker.swg#L158" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a color. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">addColor</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, c: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Color</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_PalettePicker_addHueColors"><span class="titletype">func</span> <span class="titlelight">PalettePicker.</span><span class="titlestrong">addHueColors</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\palettepicker.swg#L112" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a range of changing <code class="incode">hue</code>. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">addHueColors</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, numColors: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, sat: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, lum: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_PalettePicker_addLumColors"><span class="titletype">func</span> <span class="titlelight">PalettePicker.</span><span class="titlestrong">addLumColors</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\palettepicker.swg#L126" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a range of changing <code class="incode">luminance</code>. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">addLumColors</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, numColors: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, base: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Color</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_PalettePicker_addSatColors"><span class="titletype">func</span> <span class="titlelight">PalettePicker.</span><span class="titlestrong">addSatColors</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\palettepicker.swg#L142" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a range of changing <code class="incode">saturation</code>. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">addSatColors</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, numColors: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, base: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Color</span><span class="SyntaxCode">, lum: </span><span class="SyntaxType">f32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">0.5</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_PalettePicker_create"><span class="titletype">func</span> <span class="titlelight">PalettePicker.</span><span class="titlestrong">create</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\palettepicker.swg#L104" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">create</span><span class="SyntaxCode">(parent: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode">, position: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode"> = {}, id: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_PalettePicker">PalettePicker</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_PopupListCtrl"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">PopupListCtrl</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\popuplistctrl.swg#L14" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> frameWnd</td>
<td><a href="#Gui_FrameWnd">Gui.FrameWnd</a></td>
<td></td>
</tr>
<tr>
<td>heightItem</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>heightSeparator</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>minWidthPopup</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>maxHeightPopup</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>simFitX</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>simFitY</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>sigSelected</td>
<td>closure(*Gui.PopupListCtrl, u32)</td>
<td></td>
</tr>
<tr>
<td>selectedIdx</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>scrollWnd</td>
<td>*Gui.ScrollWnd</td>
<td></td>
</tr>
<tr>
<td>maxIconSize</td>
<td>s32</td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_PopupListCtrl_addItem">addItem(self, string, WndId, const ref Icon, *void, *void)</a></td>
<td>Add a new item in the list. </td>
</tr>
<tr>
<td><a href="#Gui_PopupListCtrl_addSeparator">addSeparator(self)</a></td>
<td>Add a separator. </td>
</tr>
<tr>
<td><a href="#Gui_PopupListCtrl_create">create(*Wnd)</a></td>
<td>Create the popup list, but do not display it. </td>
</tr>
<tr>
<td><a href="#Gui_PopupListCtrl_show">show(self, f32, f32, f32)</a></td>
<td>Show popup. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_PopupListCtrl_addItem"><span class="titletype">func</span> <span class="titlelight">PopupListCtrl.</span><span class="titlestrong">addItem</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\popuplistctrl.swg#L250" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a new item in the list. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">addItem</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, name: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, id: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">, icon: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Icon">Icon</a></span><span class="SyntaxCode"> = {}, userData0: *</span><span class="SyntaxType">void</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">, userData1: *</span><span class="SyntaxType">void</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_PopupListCtrl_addSeparator"><span class="titletype">func</span> <span class="titlelight">PopupListCtrl.</span><span class="titlestrong">addSeparator</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\popuplistctrl.swg#L268" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a separator. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">addSeparator</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_PopupListCtrl_create"><span class="titletype">func</span> <span class="titlelight">PopupListCtrl.</span><span class="titlestrong">create</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\popuplistctrl.swg#L214" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Create the popup list, but do not display it. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">create</span><span class="SyntaxCode">(owner: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_PopupListCtrl">PopupListCtrl</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_PopupListCtrl_show"><span class="titletype">func</span> <span class="titlelight">PopupListCtrl.</span><span class="titlestrong">show</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\popuplistctrl.swg#L234" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Show popup. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">show</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, x: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, width: </span><span class="SyntaxType">f32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">10</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_PopupListCtrlItem"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">PopupListCtrlItem</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\popuplistctrl.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> base</td>
<td><a href="#Gui_Item">Gui.Item</a></td>
<td></td>
</tr>
<tr>
<td>isSeparator</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>aniSel</td>
<td><a href="#Gui_BlendColor">Gui.BlendColor</a></td>
<td></td>
</tr>
<tr>
<td>aniText</td>
<td><a href="#Gui_BlendColor">Gui.BlendColor</a></td>
<td></td>
</tr>
<tr>
<td>aniCheck</td>
<td><a href="#Gui_BlendColor">Gui.BlendColor</a></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_PopupListView"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">PopupListView</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\popuplistctrl.swg#L31" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> wnd</td>
<td><a href="#Gui_Wnd">Gui.Wnd</a></td>
<td></td>
</tr>
<tr>
<td>list</td>
<td>*Gui.PopupListCtrl</td>
<td></td>
</tr>
<tr>
<td>hotIdx</td>
<td>u32</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_PopupListView_IWnd_onFocusEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onFocusEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\popuplistctrl.swg#L40" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onFocusEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_FocusEvent">FocusEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_PopupListView_IWnd_onMouseEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onMouseEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\popuplistctrl.swg#L55" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onMouseEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_MouseEvent">MouseEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_PopupListView_IWnd_onPaintEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onPaintEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\popuplistctrl.swg#L126" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onPaintEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_PaintEvent">PaintEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_PopupMenuItem"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">PopupMenuItem</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\menuctrl.swg#L17" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>rightName</td>
<td>Pixel.RichString</td>
<td></td>
</tr>
<tr>
<td>popup</td>
<td>*Gui.MenuCtrl</td>
<td></td>
</tr>
<tr>
<td>pos</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>size</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>separator</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>disabled</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>checked</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>hidden</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>aniSel</td>
<td><a href="#Gui_BlendColor">Gui.BlendColor</a></td>
<td></td>
</tr>
<tr>
<td>aniText</td>
<td><a href="#Gui_BlendColor">Gui.BlendColor</a></td>
<td></td>
</tr>
<tr>
<td>aniRightText</td>
<td><a href="#Gui_BlendColor">Gui.BlendColor</a></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_PopupPos"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">PopupPos</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\menuctrl.swg#L3" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>TopLeft</td>
<td></td>
</tr>
<tr>
<td>AnchorTopCenter</td>
<td></td>
</tr>
<tr>
<td>AnchorBottomCenter</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ProgressBar"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">ProgressBar</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\progressbar.swg#L11" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> wnd</td>
<td><a href="#Gui_Wnd">Gui.Wnd</a></td>
<td></td>
</tr>
<tr>
<td>prgBarFlags</td>
<td><a href="#Gui_ProgressBarFlags">Gui.ProgressBarFlags</a></td>
<td></td>
</tr>
<tr>
<td>progression</td>
<td>f32</td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_ProgressBar_create">create(*Wnd, const ref Rectangle, WndId)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_ProgressBar_setProgression">setProgression(self, f32)</a></td>
<td>Set the progression in [0..1] range. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ProgressBar_IWnd_onPaintEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onPaintEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\progressbar.swg#L21" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onPaintEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_PaintEvent">PaintEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ProgressBar_create"><span class="titletype">func</span> <span class="titlelight">ProgressBar.</span><span class="titlestrong">create</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\progressbar.swg#L92" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">create</span><span class="SyntaxCode">(parent: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode">, position: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode"> = {}, id: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_ProgressBar">ProgressBar</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ProgressBar_setProgression"><span class="titletype">func</span> <span class="titlelight">ProgressBar.</span><span class="titlestrong">setProgression</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\progressbar.swg#L102" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the progression in [0..1] range. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setProgression</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, prg: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ProgressBarFlags"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">ProgressBarFlags</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\progressbar.swg#L5" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>Zero</td>
<td></td>
</tr>
<tr>
<td>Infinite</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_PropertyList"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">PropertyList</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\property\propertylist.swg#L22" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> frameWnd</td>
<td><a href="#Gui_FrameWnd">Gui.FrameWnd</a></td>
<td></td>
</tr>
<tr>
<td>nameMarginBefore</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>nameHeight</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>nameMarginAfter</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>descMarginAfter</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>enumFlagsMarginBefore</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>maxSizeCombo</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>maxSizeEditValue</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>sigChanged</td>
<td>Gui.SigArray'(closure(*Gui.PropertyList))</td>
<td></td>
</tr>
<tr>
<td>font0</td>
<td>Pixel.FontFamily</td>
<td></td>
</tr>
<tr>
<td>font1</td>
<td>Pixel.FontFamily</td>
<td></td>
</tr>
<tr>
<td>font2</td>
<td>Pixel.FontFamily</td>
<td></td>
</tr>
<tr>
<td>scrollWnd</td>
<td>*Gui.ScrollWnd</td>
<td></td>
</tr>
<tr>
<td>edView</td>
<td>*Gui.PropertyListView</td>
<td></td>
</tr>
<tr>
<td>categs</td>
<td>Core.Array'({item: *Gui.PropertyListItem, lvl: s32})</td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_PropertyList_addItem">addItem(self, any)</a></td>
<td>Add a new struct to display. </td>
</tr>
<tr>
<td><a href="#Gui_PropertyList_create">create(*Wnd, const ref Rectangle, WndId)</a></td>
<td>Creates a new PropertyList instance. </td>
</tr>
<tr>
<td><a href="#Gui_PropertyList_refresh">refresh(self)</a></td>
<td>Refresh displayed values. </td>
</tr>
<tr>
<td><a href="#Gui_PropertyList_setStringFilter">setStringFilter(self, string)</a></td>
<td>Show hidden content depending on the input filter. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_PropertyList_Category"><span class="titletype">attr</span> <span class="titlelight">PropertyList.</span><span class="titlestrong">Category</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\property\propertylist.swg#L7" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="addinfos"><b>Usage</b>: all 
</div>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant"><a href="#Gui_PropertyList_Category">Category</a></span><span class="SyntaxCode">(name: </span><span class="SyntaxType">string</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_PropertyList_Description"><span class="titletype">attr</span> <span class="titlelight">PropertyList.</span><span class="titlestrong">Description</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\property\propertylist.swg#L16" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="addinfos"><b>Usage</b>: all 
</div>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant"><a href="#Gui_PropertyList_Description">Description</a></span><span class="SyntaxCode">(desc: </span><span class="SyntaxType">string</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_PropertyList_EditSlider"><span class="titletype">attr</span> <span class="titlelight">PropertyList.</span><span class="titlestrong">EditSlider</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\property\propertylist.swg#L19" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="addinfos"><b>Usage</b>: all 
</div>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant"><a href="#Gui_PropertyList_EditSlider">EditSlider</a></span><span class="SyntaxCode">(min: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, max: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_PropertyList_Name"><span class="titletype">attr</span> <span class="titlelight">PropertyList.</span><span class="titlestrong">Name</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\property\propertylist.swg#L13" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="addinfos"><b>Usage</b>: all 
</div>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant"><a href="#Gui_PropertyList_Name">Name</a></span><span class="SyntaxCode">(name: </span><span class="SyntaxType">string</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_PropertyList_SubCategory"><span class="titletype">attr</span> <span class="titlelight">PropertyList.</span><span class="titlestrong">SubCategory</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\property\propertylist.swg#L10" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="addinfos"><b>Usage</b>: all 
</div>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant"><a href="#Gui_PropertyList_SubCategory">SubCategory</a></span><span class="SyntaxCode">(name: </span><span class="SyntaxType">string</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_PropertyList_addItem"><span class="titletype">func</span> <span class="titlelight">PropertyList.</span><span class="titlestrong">addItem</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\property\propertylist.swg#L742" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a new struct to display. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">addItem</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, val: </span><span class="SyntaxType">any</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_PropertyList_create"><span class="titletype">func</span> <span class="titlelight">PropertyList.</span><span class="titlestrong">create</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\property\propertylist.swg#L700" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Creates a new PropertyList instance. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">create</span><span class="SyntaxCode">(parent: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode">, position: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode"> = {}, id: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_PropertyList">PropertyList</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_PropertyList_refresh"><span class="titletype">func</span> <span class="titlelight">PropertyList.</span><span class="titlestrong">refresh</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\property\propertylist.swg#L754" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Refresh displayed values. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">refresh</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_PropertyList_setStringFilter"><span class="titletype">func</span> <span class="titlelight">PropertyList.</span><span class="titlestrong">setStringFilter</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\property\propertylist.swg#L763" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Show hidden content depending on the input filter. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setStringFilter</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, filter: </span><span class="SyntaxType">string</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_PropertyListCtrl"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">PropertyListCtrl</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\property\propertylistctrl.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> frameWnd</td>
<td><a href="#Gui_FrameWnd">Gui.FrameWnd</a></td>
<td></td>
</tr>
<tr>
<td>splitter</td>
<td>*Gui.SplitterCtrl</td>
<td></td>
</tr>
<tr>
<td>list</td>
<td>*Gui.ListCtrl</td>
<td></td>
</tr>
<tr>
<td>prop</td>
<td>*Gui.PropertyList</td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_PropertyListCtrl_addItem">addItem(self, any)</a></td>
<td>Add a new struct to display. </td>
</tr>
<tr>
<td><a href="#Gui_PropertyListCtrl_create">create(*Wnd, const ref Rectangle, WndId)</a></td>
<td>Creates a new PropertyList instance. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_PropertyListCtrl_addItem"><span class="titletype">func</span> <span class="titlelight">PropertyListCtrl.</span><span class="titlestrong">addItem</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\property\propertylistctrl.swg#L77" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a new struct to display. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">addItem</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, val: </span><span class="SyntaxType">any</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_PropertyListCtrl_create"><span class="titletype">func</span> <span class="titlelight">PropertyListCtrl.</span><span class="titlestrong">create</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\property\propertylistctrl.swg#L17" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Creates a new PropertyList instance. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">create</span><span class="SyntaxCode">(parent: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode">, position: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode"> = {}, id: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_PropertyListCtrl">PropertyListCtrl</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_PropertyListItem"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">PropertyListItem</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\property\propertylist.swg#L45" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>filter</td>
<td>Core.String</td>
<td></td>
</tr>
<tr>
<td>line</td>
<td>*Gui.Wnd</td>
<td></td>
</tr>
<tr>
<td>ptrData</td>
<td>*u8</td>
<td></td>
</tr>
<tr>
<td>type</td>
<td>const *Swag.TypeInfo</td>
<td></td>
</tr>
<tr>
<td>typeValue</td>
<td><a href="swag.runtime.html#Swag_TypeValue">Swag.TypeValue</a></td>
<td></td>
</tr>
<tr>
<td>forceShow</td>
<td>*Gui.Wnd</td>
<td></td>
</tr>
<tr>
<td>label</td>
<td>*Gui.Label</td>
<td></td>
</tr>
<tr>
<td>labelDesc</td>
<td>*Gui.Label</td>
<td></td>
</tr>
<tr>
<td>editBox</td>
<td>*Gui.EditBox</td>
<td></td>
</tr>
<tr>
<td>checkBox</td>
<td>*Gui.CheckButton</td>
<td></td>
</tr>
<tr>
<td>comboBox</td>
<td>*Gui.ComboBox</td>
<td></td>
</tr>
<tr>
<td>sliderBox</td>
<td>*Gui.SliderCtrl</td>
<td></td>
</tr>
<tr>
<td>onResize</td>
<td>Gui.SigArray'(closure(*Gui.PropertyListItem))</td>
<td></td>
</tr>
<tr>
<td>aniFocusBk</td>
<td><a href="#Gui_BlendColor">Gui.BlendColor</a></td>
<td></td>
</tr>
<tr>
<td>aniFocusBorder</td>
<td><a href="#Gui_BlendColor">Gui.BlendColor</a></td>
<td></td>
</tr>
<tr>
<td>extendFocus</td>
<td>f32</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_PropertyListView"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">PropertyListView</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\property\propertylistview.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_PushButton"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">PushButton</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\pushbutton.swg#L12" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> button</td>
<td><a href="#Gui_Button">Gui.Button</a></td>
<td></td>
</tr>
<tr>
<td>form</td>
<td><a href="#Gui_PushButtonForm">Gui.PushButtonForm</a></td>
<td></td>
</tr>
<tr>
<td>aniBk</td>
<td><a href="#Gui_BlendColor">Gui.BlendColor</a></td>
<td></td>
</tr>
<tr>
<td>aniBorder</td>
<td><a href="#Gui_BlendColor">Gui.BlendColor</a></td>
<td></td>
</tr>
<tr>
<td>aniText</td>
<td><a href="#Gui_BlendColor">Gui.BlendColor</a></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_PushButton_create">create(*Wnd, string, const ref Rectangle, WndId)</a></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_PushButton_IWnd_onMouseEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onMouseEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\pushbutton.swg#L167" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onMouseEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_MouseEvent">MouseEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_PushButton_IWnd_onPaintEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onPaintEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\pushbutton.swg#L25" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onPaintEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_PaintEvent">PaintEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_PushButton_create"><span class="titletype">func</span> <span class="titlelight">PushButton.</span><span class="titlestrong">create</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\pushbutton.swg#L177" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">create</span><span class="SyntaxCode">(parent: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode">, name: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, position: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode"> = {}, id: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_PushButton">PushButton</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_PushButtonForm"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">PushButtonForm</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\pushbutton.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>Normal</td>
<td></td>
</tr>
<tr>
<td>Default</td>
<td></td>
</tr>
<tr>
<td>Flat</td>
<td></td>
</tr>
<tr>
<td>Strong</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_QuitEvent"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">QuitEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\events.swg#L169" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> base</td>
<td><a href="#Gui_Event">Gui.Event</a></td>
<td></td>
</tr>
<tr>
<td>quitCode</td>
<td>s32</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RadioButton"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">RadioButton</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\radiobutton.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> button</td>
<td><a href="#Gui_Button">Gui.Button</a></td>
<td></td>
</tr>
<tr>
<td>checked</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>radioButtonFlags</td>
<td><a href="#Gui_RadioButtonFlags">Gui.RadioButtonFlags</a></td>
<td></td>
</tr>
<tr>
<td>sigChanged</td>
<td>Gui.SigArray'(closure(*Gui.RadioButton))</td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_RadioButton_create">create(*Wnd, string, const ref Rectangle, WndId)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RadioButton_setChecked">setChecked(self, bool)</a></td>
<td>Set the checked state. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RadioButton_IWnd_onMouseEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onMouseEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\radiobutton.swg#L107" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onMouseEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_MouseEvent">MouseEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RadioButton_IWnd_onPaintEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onPaintEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\radiobutton.swg#L21" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onPaintEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_PaintEvent">PaintEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RadioButton_create"><span class="titletype">func</span> <span class="titlelight">RadioButton.</span><span class="titlestrong">create</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\radiobutton.swg#L121" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">create</span><span class="SyntaxCode">(parent: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode">, name: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, position: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode"> = {}, id: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_RadioButton">RadioButton</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RadioButton_setChecked"><span class="titletype">func</span> <span class="titlelight">RadioButton.</span><span class="titlestrong">setChecked</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\radiobutton.swg#L133" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the checked state. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setChecked</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, checked: </span><span class="SyntaxType">bool</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RadioButtonFlags"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">RadioButtonFlags</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\radiobutton.swg#L13" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>Zero</td>
<td></td>
</tr>
<tr>
<td>RightAlign</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ResizeEvent"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">ResizeEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\events.swg#L140" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> base</td>
<td><a href="#Gui_Event">Gui.Event</a></td>
<td></td>
</tr>
<tr>
<td>oldSize</td>
<td>Core.Math.Point</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCommand"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">RichEditCommand</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcmd.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>None</td>
<td></td>
</tr>
<tr>
<td>CursorLeft</td>
<td></td>
</tr>
<tr>
<td>CursorRight</td>
<td></td>
</tr>
<tr>
<td>CursorUp</td>
<td></td>
</tr>
<tr>
<td>CursorDown</td>
<td></td>
</tr>
<tr>
<td>CursorStartLine</td>
<td></td>
</tr>
<tr>
<td>CursorEndLine</td>
<td></td>
</tr>
<tr>
<td>CursorStartFile</td>
<td></td>
</tr>
<tr>
<td>CursorEndFile</td>
<td></td>
</tr>
<tr>
<td>CursorWordLeft</td>
<td></td>
</tr>
<tr>
<td>CursorWordRight</td>
<td></td>
</tr>
<tr>
<td>CursorPageUp</td>
<td></td>
</tr>
<tr>
<td>CursorPageDown</td>
<td>CMD_CURSOR_MATCH_PAIR. </td>
</tr>
<tr>
<td>ScrollLineUp</td>
<td></td>
</tr>
<tr>
<td>ScrollLineDown</td>
<td></td>
</tr>
<tr>
<td>DeleteLeft</td>
<td></td>
</tr>
<tr>
<td>DeleteRight</td>
<td></td>
</tr>
<tr>
<td>DeleteLine</td>
<td>CMD_EDIT_DELETE_LINE. </td>
</tr>
<tr>
<td>EditTabulation</td>
<td></td>
</tr>
<tr>
<td>EditBackTabulation</td>
<td></td>
</tr>
<tr>
<td>DeleteWordLeft</td>
<td></td>
</tr>
<tr>
<td>DeleteWordRight</td>
<td>CMD_EDIT_DELETE_LINE_LEFT CMD_EDIT_DELETE_LINE_RIGHT. </td>
</tr>
<tr>
<td>ToggleOverwrite</td>
<td></td>
</tr>
<tr>
<td>ClipboardCopy</td>
<td></td>
</tr>
<tr>
<td>ClipboardPaste</td>
<td></td>
</tr>
<tr>
<td>ClipboardCut</td>
<td></td>
</tr>
<tr>
<td>Undo</td>
<td></td>
</tr>
<tr>
<td>Redo</td>
<td></td>
</tr>
<tr>
<td>SelectRuneLeft</td>
<td></td>
</tr>
<tr>
<td>SelectRuneRight</td>
<td></td>
</tr>
<tr>
<td>SelectRuneUp</td>
<td></td>
</tr>
<tr>
<td>SelectRuneDown</td>
<td></td>
</tr>
<tr>
<td>SelectRectLeft</td>
<td></td>
</tr>
<tr>
<td>SelectRectRight</td>
<td></td>
</tr>
<tr>
<td>SelectRectUp</td>
<td></td>
</tr>
<tr>
<td>SelectRectDown</td>
<td></td>
</tr>
<tr>
<td>SelectStartLine</td>
<td></td>
</tr>
<tr>
<td>SelectEndLine</td>
<td></td>
</tr>
<tr>
<td>SelectStartFile</td>
<td></td>
</tr>
<tr>
<td>SelectEndFile</td>
<td></td>
</tr>
<tr>
<td>SelectAllFile</td>
<td></td>
</tr>
<tr>
<td>SelectWordLeft</td>
<td></td>
</tr>
<tr>
<td>SelectWordRight</td>
<td></td>
</tr>
<tr>
<td>SelectPageUp</td>
<td></td>
</tr>
<tr>
<td>SelectPageDown</td>
<td>CMD_SEL_HIDE_LINE CMD_MARKER_BOOKMARK_ADD CMD_MARKER_BOOKMARK_PREV CMD_MARKER_BOOKMARK_NEXT CMD_MARKER_BOOKMARK_FAST CMD_MAKE_UPPER CMD_MAKE_LOWER CMD_COMMENT_CPP CMD_UNCOMMENT_CPP. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">RichEditCtrl</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richedit.swg#L30" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> frameWnd</td>
<td><a href="#Gui_FrameWnd">Gui.FrameWnd</a></td>
<td></td>
</tr>
<tr>
<td>richEdFlags</td>
<td><a href="#Gui_RichEditFlags">Gui.RichEditFlags</a></td>
<td></td>
</tr>
<tr>
<td>lineSpacing</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>overwriteMode</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>tabSize</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>marginLeft</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>marginRight</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>lexer</td>
<td><a href="#Gui_IRichEditLexer">Gui.IRichEditLexer</a></td>
<td></td>
</tr>
<tr>
<td>form</td>
<td><a href="#Gui_RichEditForm">Gui.RichEditForm</a></td>
<td></td>
</tr>
<tr>
<td>sigIsReadOnly</td>
<td>Gui.SigArray'(closure(*bool))</td>
<td></td>
</tr>
<tr>
<td>sigChanged</td>
<td>Gui.SigArray'(closure(*Gui.RichEditCtrl))</td>
<td></td>
</tr>
<tr>
<td>sigModified</td>
<td>Gui.SigArray'(closure(*Gui.RichEditCtrl))</td>
<td></td>
</tr>
<tr>
<td>bkColorModel</td>
<td>Core.String</td>
<td></td>
</tr>
<tr>
<td>styles</td>
<td>[256] Gui.RichEditRuneStyle</td>
<td></td>
</tr>
<tr>
<td>stylesSelect</td>
<td>[2] Gui.RichEditRuneStyle</td>
<td></td>
</tr>
<tr>
<td>cursorPos</td>
<td><a href="#Gui_RichEditCursor">Gui.RichEditCursor</a></td>
<td></td>
</tr>
<tr>
<td>selBeg</td>
<td><a href="#Gui_RichEditCursor">Gui.RichEditCursor</a></td>
<td></td>
</tr>
<tr>
<td>lines</td>
<td>Core.ArrayPtr'(Gui.RichEditLine)</td>
<td></td>
</tr>
<tr>
<td>freeLines</td>
<td>Core.ArrayPtr'(Gui.RichEditLine)</td>
<td></td>
</tr>
<tr>
<td>scrollWnd</td>
<td>*Gui.ScrollWnd</td>
<td></td>
</tr>
<tr>
<td>edView</td>
<td>*Gui.RichEditView</td>
<td></td>
</tr>
<tr>
<td>selRectangle</td>
<td>Core.Math.Rectangle</td>
<td></td>
</tr>
<tr>
<td>hasSelRect</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>modified</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>recomputeScroll</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>dirtyFirstLineView</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>forceCaretVisible</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>forceCaretVisibleCenter</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>undoMgr</td>
<td>Core.ArrayPtr'(Gui.RichEditUndo)</td>
<td></td>
</tr>
<tr>
<td>toUndo</td>
<td>Core.ArrayPtr'(Gui.RichEditUndo)</td>
<td></td>
</tr>
<tr>
<td>mapping</td>
<td>Core.Array'(Gui.RichEditMapping)</td>
<td></td>
</tr>
<tr>
<td>currentUndo</td>
<td>u64</td>
<td></td>
</tr>
<tr>
<td>undoCounter</td>
<td>u64</td>
<td></td>
</tr>
<tr>
<td>modifiedUndoMarker</td>
<td>u64</td>
<td></td>
</tr>
<tr>
<td>serialUndo</td>
<td>u64</td>
<td></td>
</tr>
<tr>
<td>countVisibleLines</td>
<td>u64</td>
<td></td>
</tr>
<tr>
<td>firstVisibleLineView</td>
<td>u64</td>
<td></td>
</tr>
<tr>
<td>lastVisibleLineView</td>
<td>u64</td>
<td></td>
</tr>
<tr>
<td>clipFormat</td>
<td>Gui.Clipboard.Format</td>
<td></td>
</tr>
<tr>
<td>cursorPosUndo</td>
<td><a href="#Gui_RichEditCursor">Gui.RichEditCursor</a></td>
<td></td>
</tr>
<tr>
<td>selBegUndo</td>
<td><a href="#Gui_RichEditCursor">Gui.RichEditCursor</a></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_RichEditCtrl_addMapping">addMapping(self, Key, KeyModifiers, RichEditCommand)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_addText">addText(self, string, RichEditStyleRef)</a></td>
<td>Append a text with a given style. </td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_canRedo">canRedo(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_canUndo">canUndo(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_charPressed">charPressed(self, Key, rune, KeyModifiers)</a></td>
<td>Simulate a keyboard character pressed. </td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_checkSelection">checkSelection(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_clear">clear(self)</a></td>
<td>Clear content. </td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_clearMapping">clearMapping(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_clearSelection">clearSelection(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_clearUndo">clearUndo(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_copyToClipboard">copyToClipboard(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_create">create(*Wnd, const ref Rectangle, WndId)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_deleteBegLine">deleteBegLine(self, string, bool)</a></td>
<td>Delete a given text at the start of each selected line. </td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_deleteLeft">deleteLeft(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_deleteLine">deleteLine(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_deleteRight">deleteRight(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_deleteSelection">deleteSelection(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_deleteWordLeft">deleteWordLeft(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_deleteWordRight">deleteWordRight(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_ensureCaretIsVisible">ensureCaretIsVisible(self, bool)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_ensureCursorIsVisible">ensureCursorIsVisible(self, const ref RichEditCursor, bool)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_evaluateSize">evaluateSize(self)</a></td>
<td>Evaluate the rendering size. </td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_executeCommand">executeCommand(self, RichEditCommand)</a></td>
<td>Execute a command. </td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_getBegSelection">getBegSelection(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_getCaretPos">getCaretPos(self, const ref RichEditCursor)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_getCursorDisplayPos">getCursorDisplayPos(self, const ref RichEditCursor)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_getCursorPosFromPoint">getCursorPosFromPoint(self, const ref Point)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_getEndSelection">getEndSelection(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_getMappingCommand">getMappingCommand(self, Key, KeyModifiers)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_getRune">getRune(self, const ref RichEditCursor)</a></td>
<td>Get the rune at the given cursor pos. </td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_getRuneSize">getRuneSize(self, *RichEditLine, u64, f32)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_getSelectedText">getSelectedText(self)</a></td>
<td>Get the selected text. </td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_getTabOffset">getTabOffset(self, s32, *s32)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_getText">getText(self)</a></td>
<td>Get the full text. </td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_getTextAndStyles">getTextAndStyles(self)</a></td>
<td>Get the full text with the associated styles per character. </td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_getXExtent">getXExtent(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_insertBegLine">insertBegLine(self, string)</a></td>
<td>Insert text at the start of each selected line. </td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_insertRune">insertRune(self, rune)</a></td>
<td>Insert a rune. </td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_insertText">insertText(self, string)</a></td>
<td>Insert a text with a given style. </td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_insertText">insertText(self, string, const ref Array'(u8))</a></td>
<td>Insert a text with a style per character. </td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_isReadOnly">isReadOnly(self)</a></td>
<td>Returns true if the rich edit is read only. </td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_isSelectionEmpty">isSelectionEmpty(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_keyPressed">keyPressed(self, Key, KeyModifiers)</a></td>
<td>Simulate a keyboard pressed. </td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_lexAll">lexAll(self)</a></td>
<td>Lex the full text. </td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_moveCursorDown">moveCursorDown(self, *RichEditCursor)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_moveCursorEndFile">moveCursorEndFile(self, *RichEditCursor)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_moveCursorEndLine">moveCursorEndLine(self, *RichEditCursor)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_moveCursorLeft">moveCursorLeft(self, *RichEditCursor)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_moveCursorPageDown">moveCursorPageDown(self, *RichEditCursor)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_moveCursorPageUp">moveCursorPageUp(self, *RichEditCursor)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_moveCursorRight">moveCursorRight(self, *RichEditCursor)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_moveCursorStartFile">moveCursorStartFile(self, *RichEditCursor)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_moveCursorStartLine">moveCursorStartLine(self, *RichEditCursor)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_moveCursorUp">moveCursorUp(self, *RichEditCursor)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_moveCursorWordLeft">moveCursorWordLeft(self, *RichEditCursor)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_moveCursorWordRight">moveCursorWordRight(self, *RichEditCursor, bool)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_pasteFromClipboard">pasteFromClipboard(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_popUndo">popUndo(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_pushUndo">pushUndo(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_redo">redo(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_scrollLineDown">scrollLineDown(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_scrollLineUp">scrollLineUp(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_selectAll">selectAll(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_selectWord">selectWord(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_setCursorPos">setCursorPos(self, const ref RichEditCursor)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_setDefaultMapping">setDefaultMapping(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_setFocus">setFocus(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_setForm">setForm(self, RichEditForm)</a></td>
<td>Set form. </td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_setLexer">setLexer(self, IRichEditLexer)</a></td>
<td>Associate a lexer. </td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_setRectangularSelection">setRectangularSelection(self, bool)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_setRichEdFlags">setRichEdFlags(self, RichEditFlags)</a></td>
<td>Set richedit flags. </td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_setSelection">setSelection(self, const ref RichEditCursor, const ref RichEditCursor)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_setText">setText(self, string)</a></td>
<td>Set the text content. </td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_undo">undo(self)</a></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_addMapping"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">addMapping</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcmd.swg#L73" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">addMapping</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, key: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Input</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Key</span><span class="SyntaxCode">, mdf: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Input</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">KeyModifiers</span><span class="SyntaxCode">, cmd: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_RichEditCommand">RichEditCommand</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_addText"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">addText</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richedittext.swg#L447" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Append a text with a given style. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">addText</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, text: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, txtStyle: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">RichEditStyleRef</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_canRedo"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">canRedo</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditundo.swg#L158" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">canRedo</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxType">bool</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_canUndo"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">canUndo</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditundo.swg#L104" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">canUndo</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxType">bool</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_charPressed"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">charPressed</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richedit.swg#L346" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Simulate a keyboard character pressed. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">charPressed</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, key: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Input</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Key</span><span class="SyntaxCode">, ch: </span><span class="SyntaxType">rune</span><span class="SyntaxCode">, mdf: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Input</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">KeyModifiers</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_checkSelection"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">checkSelection</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richesitselection.swg#L154" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">checkSelection</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_clear"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">clear</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richedittext.swg#L390" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Clear content. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">clear</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_clearMapping"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">clearMapping</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcmd.swg#L68" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">clearMapping</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_clearSelection"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">clearSelection</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richesitselection.swg#L94" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">clearSelection</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_clearUndo"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">clearUndo</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditundo.swg#L62" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">clearUndo</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_copyToClipboard"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">copyToClipboard</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richesitselection.swg#L266" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">copyToClipboard</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_create"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">create</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richedit.swg#L242" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">create</span><span class="SyntaxCode">(parent: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode">, position: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode"> = {}, id: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_RichEditCtrl">RichEditCtrl</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_deleteBegLine"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">deleteBegLine</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richedittext.swg#L690" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Delete a given text at the start of each selected line. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">deleteBegLine</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, txt: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, lastLine: </span><span class="SyntaxType">bool</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_deleteLeft"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">deleteLeft</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richesitselection.swg#L205" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">deleteLeft</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_deleteLine"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">deleteLine</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richesitselection.swg#L299" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">deleteLine</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_deleteRight"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">deleteRight</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richesitselection.swg#L228" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">deleteRight</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_deleteSelection"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">deleteSelection</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richesitselection.swg#L179" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">deleteSelection</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_deleteWordLeft"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">deleteWordLeft</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richesitselection.swg#L241" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">deleteWordLeft</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_deleteWordRight"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">deleteWordRight</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richesitselection.swg#L251" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">deleteWordRight</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_ensureCaretIsVisible"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">ensureCaretIsVisible</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcursor.swg#L91" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">ensureCaretIsVisible</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, center = </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_ensureCursorIsVisible"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">ensureCursorIsVisible</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcursor.swg#L98" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">ensureCursorIsVisible</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, cp: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_RichEditCursor">RichEditCursor</a></span><span class="SyntaxCode">, center: </span><span class="SyntaxType">bool</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_evaluateSize"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">evaluateSize</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richedittext.swg#L796" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Evaluate the rendering size. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">evaluateSize</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode">
{ 
	w: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">
	h: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">
}</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_executeCommand"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">executeCommand</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcmd.swg#L145" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Execute a command. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">executeCommand</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, cmd: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_RichEditCommand">RichEditCommand</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_getBegSelection"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">getBegSelection</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richesitselection.swg#L116" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getBegSelection</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_RichEditCursor">RichEditCursor</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_getCaretPos"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">getCaretPos</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcursor.swg#L71" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getCaretPos</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, cp: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_RichEditCursor">RichEditCursor</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_getCursorDisplayPos"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">getCursorDisplayPos</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcursor.swg#L31" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getCursorDisplayPos</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, cp: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_RichEditCursor">RichEditCursor</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Point</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_getCursorPosFromPoint"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">getCursorPosFromPoint</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcursor.swg#L135" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getCursorPosFromPoint</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, pt: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Point</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_RichEditCursor">RichEditCursor</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_getEndSelection"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">getEndSelection</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richesitselection.swg#L135" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getEndSelection</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_RichEditCursor">RichEditCursor</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_getMappingCommand"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">getMappingCommand</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcmd.swg#L82" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getMappingCommand</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, key: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Input</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Key</span><span class="SyntaxCode">, mdf: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Input</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">KeyModifiers</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_RichEditCommand">RichEditCommand</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_getRune"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">getRune</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richedittext.swg#L497" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the rune at the given cursor pos. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getRune</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, cp: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_RichEditCursor">RichEditCursor</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode">
{ 
	char: </span><span class="SyntaxType">rune</span><span class="SyntaxCode">
	style: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">RichEditStyleRef</span><span class="SyntaxCode">
}</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_getRuneSize"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">getRuneSize</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcursor.swg#L12" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getRuneSize</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, line: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_RichEditLine">RichEditLine</a></span><span class="SyntaxCode">, i: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">, x: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Point</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_getSelectedText"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">getSelectedText</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richedittext.swg#L405" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the selected text. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getSelectedText</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">String</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_getTabOffset"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">getTabOffset</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcursor.swg#L49" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getTabOffset</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, numBlanks: *</span><span class="SyntaxType">s32</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_getText"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">getText</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richedittext.swg#L413" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the full text. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getText</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">String</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_getTextAndStyles"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">getTextAndStyles</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richedittext.swg#L425" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the full text with the associated styles per character. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getTextAndStyles</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode">
{ 
	text: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Array</span><span class="SyntaxCode">'(</span><span class="SyntaxType">rune</span><span class="SyntaxCode">)
	styles: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Array</span><span class="SyntaxCode">'(</span><span class="SyntaxType">u8</span><span class="SyntaxCode">)
}</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_getXExtent"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">getXExtent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcursor.swg#L65" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getXExtent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Point</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_insertBegLine"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">insertBegLine</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richedittext.swg#L637" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Insert text at the start of each selected line. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">insertBegLine</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, txt: </span><span class="SyntaxType">string</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_insertRune"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">insertRune</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richedittext.swg#L513" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Insert a rune. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">insertRune</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, ch: </span><span class="SyntaxType">rune</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_insertText"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">insertText</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richedittext.swg#L461" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Insert a text with a given style. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">insertText</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, text: </span><span class="SyntaxType">string</span><span class="SyntaxCode">)</code>
</pre>
<p>Insert a text with a style per character. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">insertText</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, text: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, txtStyles: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Array</span><span class="SyntaxCode">'(</span><span class="SyntaxType">u8</span><span class="SyntaxCode">))</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_isReadOnly"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">isReadOnly</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richedit.swg#L338" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns true if the rich edit is read only. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">isReadOnly</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxType">bool</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_isSelectionEmpty"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">isSelectionEmpty</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richesitselection.swg#L107" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">isSelectionEmpty</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxType">bool</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_keyPressed"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">keyPressed</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richedit.swg#L378" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Simulate a keyboard pressed. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">keyPressed</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, key: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Input</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Key</span><span class="SyntaxCode">, mdf: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Input</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">KeyModifiers</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_lexAll"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">lexAll</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richedit.swg#L396" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Lex the full text. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">lexAll</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_moveCursorDown"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">moveCursorDown</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcursor.swg#L306" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">moveCursorDown</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, cp: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_RichEditCursor">RichEditCursor</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">bool</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_moveCursorEndFile"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">moveCursorEndFile</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcursor.swg#L392" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">moveCursorEndFile</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, cp: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_RichEditCursor">RichEditCursor</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_moveCursorEndLine"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">moveCursorEndLine</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcursor.swg#L381" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">moveCursorEndLine</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, cp: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_RichEditCursor">RichEditCursor</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_moveCursorLeft"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">moveCursorLeft</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcursor.swg#L226" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">moveCursorLeft</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, cp: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_RichEditCursor">RichEditCursor</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">bool</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_moveCursorPageDown"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">moveCursorPageDown</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcursor.swg#L411" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">moveCursorPageDown</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, cp: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_RichEditCursor">RichEditCursor</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_moveCursorPageUp"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">moveCursorPageUp</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcursor.swg#L398" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">moveCursorPageUp</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, cp: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_RichEditCursor">RichEditCursor</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_moveCursorRight"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">moveCursorRight</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcursor.swg#L245" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">moveCursorRight</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, cp: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_RichEditCursor">RichEditCursor</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">bool</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_moveCursorStartFile"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">moveCursorStartFile</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcursor.swg#L386" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">moveCursorStartFile</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, cp: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_RichEditCursor">RichEditCursor</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_moveCursorStartLine"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">moveCursorStartLine</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcursor.swg#L347" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">moveCursorStartLine</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, cp: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_RichEditCursor">RichEditCursor</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_moveCursorUp"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">moveCursorUp</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcursor.swg#L264" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">moveCursorUp</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, cp: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_RichEditCursor">RichEditCursor</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">bool</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_moveCursorWordLeft"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">moveCursorWordLeft</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcursor.swg#L424" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">moveCursorWordLeft</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, cp: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_RichEditCursor">RichEditCursor</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_moveCursorWordRight"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">moveCursorWordRight</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcursor.swg#L473" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">moveCursorWordRight</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, cp: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_RichEditCursor">RichEditCursor</a></span><span class="SyntaxCode">, withSpace: </span><span class="SyntaxType">bool</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_pasteFromClipboard"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">pasteFromClipboard</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richesitselection.swg#L279" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">pasteFromClipboard</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_popUndo"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">popUndo</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditundo.swg#L84" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">popUndo</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_pushUndo"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">pushUndo</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditundo.swg#L71" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">pushUndo</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_redo"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">redo</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditundo.swg#L167" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">redo</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_scrollLineDown"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">scrollLineDown</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richesitselection.swg#L392" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">scrollLineDown</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_scrollLineUp"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">scrollLineUp</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richesitselection.swg#L370" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">scrollLineUp</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_selectAll"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">selectAll</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richesitselection.swg#L82" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">selectAll</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_selectWord"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">selectWord</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richesitselection.swg#L316" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">selectWord</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_setCursorPos"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">setCursorPos</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcursor.swg#L220" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setCursorPos</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, cp: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_RichEditCursor">RichEditCursor</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_setDefaultMapping"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">setDefaultMapping</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcmd.swg#L93" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setDefaultMapping</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_setFocus"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">setFocus</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richedit.swg#L274" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setFocus</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_setForm"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">setForm</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richedit.swg#L288" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set form. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setForm</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, form: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_RichEditForm">RichEditForm</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_setLexer"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">setLexer</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richedit.swg#L389" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Associate a lexer. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setLexer</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, lex: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_IRichEditLexer">IRichEditLexer</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_setRectangularSelection"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">setRectangularSelection</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richesitselection.swg#L89" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setRectangularSelection</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, selRect: </span><span class="SyntaxType">bool</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_setRichEdFlags"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">setRichEdFlags</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richedit.swg#L280" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set richedit flags. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setRichEdFlags</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, flags: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_RichEditFlags">RichEditFlags</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_setSelection"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">setSelection</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richesitselection.swg#L100" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setSelection</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, start: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_RichEditCursor">RichEditCursor</a></span><span class="SyntaxCode">, end: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_RichEditCursor">RichEditCursor</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_setText"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">setText</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richedittext.swg#L435" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the text content. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setText</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, text: </span><span class="SyntaxType">string</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCtrl_undo"><span class="titletype">func</span> <span class="titlelight">RichEditCtrl.</span><span class="titlestrong">undo</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditundo.swg#L113" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">undo</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditCursor"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">RichEditCursor</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcursor.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>lineIndex</td>
<td>u64</td>
<td></td>
</tr>
<tr>
<td>charPosInLine</td>
<td>u64</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditFlags"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">RichEditFlags</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richedit.swg#L5" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>Zero</td>
<td></td>
</tr>
<tr>
<td>ReadOnly</td>
<td></td>
</tr>
<tr>
<td>ShowFocusSel</td>
<td></td>
</tr>
<tr>
<td>CurLineBorder</td>
<td></td>
</tr>
<tr>
<td>DrawBlanks</td>
<td></td>
</tr>
<tr>
<td>TabToSpaces</td>
<td></td>
</tr>
<tr>
<td>WordWrap</td>
<td></td>
</tr>
<tr>
<td>AutoLoseFocus</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditForm"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">RichEditForm</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richedit.swg#L17" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>RichEditCtrl</td>
<td></td>
</tr>
<tr>
<td>Edit</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditLexerSwag"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">RichEditLexerSwag</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditlexerswag.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>mapStyles</td>
<td>[256] Gui.RichEditStyleRef</td>
<td></td>
</tr>
<tr>
<td>mapWords</td>
<td>Core.HashTable'(string, s32)</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditLexerSwag_IRichEditLexer_compute"><span class="titletype">func</span> <span class="titlelight">IRichEditLexer.</span><span class="titlestrong">compute</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditlexerswag.swg#L413" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">compute</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, linePrev: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_RichEditLine">RichEditLine</a></span><span class="SyntaxCode">, line: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_RichEditLine">RichEditLine</a></span><span class="SyntaxCode">, lineNext: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_RichEditLine">RichEditLine</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditLexerSwag_IRichEditLexer_insertRune"><span class="titletype">func</span> <span class="titlelight">IRichEditLexer.</span><span class="titlestrong">insertRune</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditlexerswag.swg#L381" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">insertRune</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, ed: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_RichEditCtrl">RichEditCtrl</a></span><span class="SyntaxCode">, ch: </span><span class="SyntaxType">rune</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">bool</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditLexerSwag_IRichEditLexer_setup"><span class="titletype">func</span> <span class="titlelight">IRichEditLexer.</span><span class="titlestrong">setup</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditlexerswag.swg#L62" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setup</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, ed: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_RichEditCtrl">RichEditCtrl</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditLine"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">RichEditLine</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditline.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditMapping"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">RichEditMapping</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richedit.swg#L23" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>key</td>
<td>Core.Input.Key</td>
<td></td>
</tr>
<tr>
<td>modifiers</td>
<td>Core.Input.KeyModifiers</td>
<td></td>
</tr>
<tr>
<td>command</td>
<td><a href="#Gui_RichEditCommand">Gui.RichEditCommand</a></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditRuneStyle"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">RichEditRuneStyle</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcharstyle.swg#L6" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>colBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>colFg</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>fontMode</td>
<td>Pixel.FontFamilyStyle</td>
<td></td>
</tr>
<tr>
<td>underline</td>
<td>Pixel.UnderlineStyle</td>
<td></td>
</tr>
</table>
<h3>Special Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_RichEditRuneStyle_opEquals">opEquals(self, const ref RichEditRuneStyle)</a></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditRuneStyle_opEquals"><span class="titletype">func</span> <span class="titlelight">RichEditRuneStyle.</span><span class="titlestrong">opEquals</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcharstyle.swg#L16" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">opEquals</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, other: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_RichEditRuneStyle">RichEditRuneStyle</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">bool</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditUndo"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">RichEditUndo</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditundo.swg#L13" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>type</td>
<td><a href="#Gui_RichEditUndoType">Gui.RichEditUndoType</a></td>
<td></td>
</tr>
<tr>
<td>concatChars</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>selBeg</td>
<td><a href="#Gui_RichEditCursor">Gui.RichEditCursor</a></td>
<td></td>
</tr>
<tr>
<td>cursorPos</td>
<td><a href="#Gui_RichEditCursor">Gui.RichEditCursor</a></td>
<td></td>
</tr>
<tr>
<td>runes</td>
<td>Core.Array'(rune)</td>
<td></td>
</tr>
<tr>
<td>styles</td>
<td>Core.Array'(u8)</td>
<td></td>
</tr>
<tr>
<td>serial</td>
<td>u64</td>
<td></td>
</tr>
<tr>
<td>hasSelRect</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>begSelectionBefore</td>
<td><a href="#Gui_RichEditCursor">Gui.RichEditCursor</a></td>
<td></td>
</tr>
<tr>
<td>endSelectionBefore</td>
<td><a href="#Gui_RichEditCursor">Gui.RichEditCursor</a></td>
<td></td>
</tr>
<tr>
<td>begSelectionAfter</td>
<td><a href="#Gui_RichEditCursor">Gui.RichEditCursor</a></td>
<td></td>
</tr>
<tr>
<td>endSelectionAfter</td>
<td><a href="#Gui_RichEditCursor">Gui.RichEditCursor</a></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditUndoType"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">RichEditUndoType</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditundo.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>Delete</td>
<td></td>
</tr>
<tr>
<td>Insert</td>
<td></td>
</tr>
<tr>
<td>Selection</td>
<td></td>
</tr>
<tr>
<td>Show</td>
<td></td>
</tr>
<tr>
<td>Hide</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_RichEditView"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">RichEditView</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditview.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ScrollWnd"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">ScrollWnd</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\scrollwnd.swg#L13" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> frameWnd</td>
<td><a href="#Gui_FrameWnd">Gui.FrameWnd</a></td>
<td></td>
</tr>
<tr>
<td>scrollIncrementV</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>scrollIncrementH</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>scrollSize</td>
<td>Core.Math.Point</td>
<td></td>
</tr>
<tr>
<td>scrollWndFlags</td>
<td><a href="#Gui_ScrollWndFlags">Gui.ScrollWndFlags</a></td>
<td></td>
</tr>
<tr>
<td>sigScrollPosChanged</td>
<td>Gui.SigArray'(closure(*Gui.ScrollWnd, Core.Math.Point, Core.Math.Point))</td>
<td></td>
</tr>
<tr>
<td>sigScrollSizeChanged</td>
<td>Gui.SigArray'(closure(*Gui.ScrollWnd, Core.Math.Point, Core.Math.Point))</td>
<td></td>
</tr>
<tr>
<td>oldMouseCapture</td>
<td>*Gui.Wnd</td>
<td></td>
</tr>
<tr>
<td>scrollSmoothPos</td>
<td>Core.Math.Point</td>
<td></td>
</tr>
<tr>
<td>scrollRequestPos</td>
<td>Core.Math.Point</td>
<td></td>
</tr>
<tr>
<td>smoothScroll</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>showBarV</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>needV</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>posBoxV</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>sizeBoxV</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>isHotV</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>isHotBarV</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>isPressedV</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>showBarH</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>needH</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>posBoxH</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>sizeBoxH</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>isHotH</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>isHotBarH</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>isPressedH</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>isMoving</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>isGrabbing</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>correcMoving</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>offsetClientV</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>offsetClientH</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>paddingZ</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>paddingW</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>aniBkBarV</td>
<td><a href="#Gui_BlendColor">Gui.BlendColor</a></td>
<td></td>
</tr>
<tr>
<td>aniBkBarH</td>
<td><a href="#Gui_BlendColor">Gui.BlendColor</a></td>
<td></td>
</tr>
<tr>
<td>aniBoxV</td>
<td><a href="#Gui_BlendColor">Gui.BlendColor</a></td>
<td></td>
</tr>
<tr>
<td>aniBoxH</td>
<td><a href="#Gui_BlendColor">Gui.BlendColor</a></td>
<td></td>
</tr>
<tr>
<td>grabbingPos</td>
<td>Core.Math.Point</td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_ScrollWnd_getClientScrollRect">getClientScrollRect(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_ScrollWnd_getScrollPos">getScrollPos(self)</a></td>
<td>Get the current scroll position. </td>
</tr>
<tr>
<td><a href="#Gui_ScrollWnd_setScrollPos">setScrollPos(self, const ref Point)</a></td>
<td>Set the current scroll position. </td>
</tr>
<tr>
<td><a href="#Gui_ScrollWnd_setScrollPos">setScrollPos(self, f32, f32)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_ScrollWnd_setScrollSize">setScrollSize(self, f32, f32)</a></td>
<td>Set the scroll size (window virtual size). </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ScrollWnd_IWnd_onMouseEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onMouseEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\scrollwnd.swg#L78" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onMouseEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_MouseEvent">MouseEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ScrollWnd_IWnd_onPaintEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onPaintEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\scrollwnd.swg#L197" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onPaintEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_PaintEvent">PaintEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ScrollWnd_IWnd_onResizeEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onResizeEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\scrollwnd.swg#L66" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onResizeEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_ResizeEvent">ResizeEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ScrollWnd_getClientScrollRect"><span class="titletype">func</span> <span class="titlelight">ScrollWnd.</span><span class="titlestrong">getClientScrollRect</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\scrollwnd.swg#L514" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getClientScrollRect</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ScrollWnd_getScrollPos"><span class="titletype">func</span> <span class="titlelight">ScrollWnd.</span><span class="titlestrong">getScrollPos</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\scrollwnd.swg#L454" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the current scroll position. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getScrollPos</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Point</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ScrollWnd_setScrollPos"><span class="titletype">func</span> <span class="titlelight">ScrollWnd.</span><span class="titlestrong">setScrollPos</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\scrollwnd.swg#L489" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the current scroll position. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setScrollPos</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, pt: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Point</span><span class="SyntaxCode">)</code>
</pre>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setScrollPos</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, x: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ScrollWnd_setScrollSize"><span class="titletype">func</span> <span class="titlelight">ScrollWnd.</span><span class="titlestrong">setScrollSize</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\scrollwnd.swg#L502" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the scroll size (window virtual size). </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setScrollSize</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, x: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ScrollWndFlags"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">ScrollWndFlags</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\scrollwnd.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>Zero</td>
<td></td>
</tr>
<tr>
<td>SmallBar</td>
<td></td>
</tr>
<tr>
<td>ForceHorizontal</td>
<td></td>
</tr>
<tr>
<td>ForceVertical</td>
<td></td>
</tr>
<tr>
<td>TransparentBar</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_SelModel"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">SelModel</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\selmodel.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>selection</td>
<td>Core.Array'({start: u32, end: u32})</td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_SelModel_getSelectedRangeIndex">getSelectedRangeIndex(self, u32)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_SelModel_isEmpty">isEmpty(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_SelModel_isSelected">isSelected(self, u32)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_SelModel_select">select(self, u32, bool)</a></td>
<td>Select the given index. </td>
</tr>
<tr>
<td><a href="#Gui_SelModel_unselectAll">unselectAll(self)</a></td>
<td>Unselect all. </td>
</tr>
</table>
<h3>Special Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_SelModel_opVisit">opVisit(self, code)</a></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_SelModel_getSelectedRangeIndex"><span class="titletype">func</span> <span class="titlelight">SelModel.</span><span class="titlestrong">getSelectedRangeIndex</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\selmodel.swg#L11" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getSelectedRangeIndex</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, idx: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u32</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_SelModel_isEmpty"><span class="titletype">func</span> <span class="titlelight">SelModel.</span><span class="titlestrong">isEmpty</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\selmodel.swg#L24" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">isEmpty</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_SelModel_isSelected"><span class="titletype">func</span> <span class="titlelight">SelModel.</span><span class="titlestrong">isSelected</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\selmodel.swg#L25" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">isSelected</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, idx: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_SelModel_opVisit"><span class="titletype">func</span> <span class="titlelight">SelModel.</span><span class="titlestrong">opVisit</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\selmodel.swg#L82" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxAttribute">#[<a href="swag.runtime.html#Swag_Macro">Swag.Macro</a>]</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(ptr: </span><span class="SyntaxType">bool</span><span class="SyntaxCode">) </span><span class="SyntaxFunction">opVisit</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, stmt: </span><span class="SyntaxType">code</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_SelModel_select"><span class="titletype">func</span> <span class="titlelight">SelModel.</span><span class="titlestrong">select</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\selmodel.swg#L35" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Select the given index. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">select</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, idx: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">, select: </span><span class="SyntaxType">bool</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_SelModel_unselectAll"><span class="titletype">func</span> <span class="titlelight">SelModel.</span><span class="titlestrong">unselectAll</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\selmodel.swg#L29" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Unselect all. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">unselectAll</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_SerializeStateEvent"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">SerializeStateEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\events.swg#L212" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> base</td>
<td><a href="#Gui_Event">Gui.Event</a></td>
<td></td>
</tr>
<tr>
<td>ser</td>
<td>*Core.Serialization.Serializer</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_SetThemeEvent"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">SetThemeEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\events.swg#L205" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> base</td>
<td><a href="#Gui_Event">Gui.Event</a></td>
<td></td>
</tr>
<tr>
<td>style</td>
<td>*Gui.ThemeStyle</td>
<td></td>
</tr>
<tr>
<td>parent</td>
<td>*Gui.ThemeStyle</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_SigArray"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">SigArray</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\sigarray.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode"> </span><span class="SyntaxConstant"><a href="#Gui_SigArray">SigArray</a></span><span class="SyntaxCode">(</span><span class="SyntaxConstant">T</span><span class="SyntaxCode">)</code>
</pre>
<table class="enumeration">
<tr>
<td>arr</td>
<td>Core.Array'(T)</td>
<td></td>
</tr>
<tr>
<td>disabled</td>
<td>s32</td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_SigArray_call">call(self, ...)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_SigArray_disable">disable(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_SigArray_enable">enable(self)</a></td>
<td></td>
</tr>
</table>
<h3>Special Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_SigArray_opAssign">opAssign(self, T)</a></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_SigArray_call"><span class="titletype">func</span> <span class="titlelight">SigArray.</span><span class="titlestrong">call</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\sigarray.swg#L29" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">call</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, params: ...)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_SigArray_disable"><span class="titletype">func</span> <span class="titlelight">SigArray.</span><span class="titlestrong">disable</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\sigarray.swg#L18" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">disable</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_SigArray_enable"><span class="titletype">func</span> <span class="titlelight">SigArray.</span><span class="titlestrong">enable</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\sigarray.swg#L23" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">enable</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_SigArray_opAssign"><span class="titletype">func</span> <span class="titlelight">SigArray.</span><span class="titlestrong">opAssign</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\sigarray.swg#L12" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(op: </span><span class="SyntaxType">string</span><span class="SyntaxCode">) </span><span class="SyntaxFunction">opAssign</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, fn: </span><span class="SyntaxConstant">T</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_SizingBorder"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">SizingBorder</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\surfacewnd.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>Zero</td>
<td></td>
</tr>
<tr>
<td>Left</td>
<td></td>
</tr>
<tr>
<td>Top</td>
<td></td>
</tr>
<tr>
<td>Right</td>
<td></td>
</tr>
<tr>
<td>Bottom</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Slider"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">Slider</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\slider.swg#L14" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> wnd</td>
<td><a href="#Gui_Wnd">Gui.Wnd</a></td>
<td></td>
</tr>
<tr>
<td>value1</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>value2</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>min</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>max</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>steps</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>sliderFlags</td>
<td><a href="#Gui_SliderFlags">Gui.SliderFlags</a></td>
<td></td>
</tr>
<tr>
<td>sigStartChange</td>
<td>Gui.SigArray'(closure(*Gui.Slider))</td>
<td></td>
</tr>
<tr>
<td>sigChanged</td>
<td>Gui.SigArray'(closure(*Gui.Slider))</td>
<td></td>
</tr>
<tr>
<td>sigEndChange</td>
<td>Gui.SigArray'(closure(*Gui.Slider))</td>
<td></td>
</tr>
<tr>
<td>isMoving</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>isPressed1</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>isPressed2</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>isHot1</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>isHot2</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>hasValue1</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>hasValue2</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>correcMoving</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>aniMark1</td>
<td><a href="#Gui_BlendColor">Gui.BlendColor</a></td>
<td></td>
</tr>
<tr>
<td>aniMark2</td>
<td><a href="#Gui_BlendColor">Gui.BlendColor</a></td>
<td></td>
</tr>
<tr>
<td>aniStep1</td>
<td><a href="#Gui_BlendColor">Gui.BlendColor</a></td>
<td></td>
</tr>
<tr>
<td>aniStep2</td>
<td><a href="#Gui_BlendColor">Gui.BlendColor</a></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_Slider_create">create(*Wnd, const ref Rectangle, WndId, bool, bool)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Slider_getValue">getValue(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Slider_getValueLeft">getValueLeft(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Slider_getValueRight">getValueRight(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Slider_setMinMax">setMinMax(self, f32, f32, f32)</a></td>
<td>Set the <code class="incode">min</code>, <code class="incode">max</code> and <code class="incode">steps</code> values. </td>
</tr>
<tr>
<td><a href="#Gui_Slider_setRangeValues">setRangeValues(self, f32, f32)</a></td>
<td>Set left and right values, for a range. </td>
</tr>
<tr>
<td><a href="#Gui_Slider_setValue">setValue(self, f32)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Slider_setValueLeft">setValueLeft(self, f32)</a></td>
<td>Set the left value (in case of a range). </td>
</tr>
<tr>
<td><a href="#Gui_Slider_setValueRight">setValueRight(self, f32)</a></td>
<td>Set the right value (in case of a range). </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Slider_IWnd_onMouseEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onMouseEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\slider.swg#L201" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onMouseEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_MouseEvent">MouseEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Slider_IWnd_onPaintEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onPaintEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\slider.swg#L45" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onPaintEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_PaintEvent">PaintEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Slider_create"><span class="titletype">func</span> <span class="titlelight">Slider.</span><span class="titlestrong">create</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\slider.swg#L506" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">create</span><span class="SyntaxCode">(parent: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode">, position: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode"> = {}, id: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">, range = </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">, vertical = </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Slider">Slider</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Slider_getValue"><span class="titletype">func</span> <span class="titlelight">Slider.</span><span class="titlestrong">getValue</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\slider.swg#L461" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getValue</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Slider_getValueLeft"><span class="titletype">func</span> <span class="titlelight">Slider.</span><span class="titlestrong">getValueLeft</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\slider.swg#L462" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getValueLeft</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Slider_getValueRight"><span class="titletype">func</span> <span class="titlelight">Slider.</span><span class="titlestrong">getValueRight</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\slider.swg#L463" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getValueRight</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Slider_setMinMax"><span class="titletype">func</span> <span class="titlelight">Slider.</span><span class="titlestrong">setMinMax</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\slider.swg#L484" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the <code class="incode">min</code>, <code class="incode">max</code> and <code class="incode">steps</code> values. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setMinMax</span><span class="SyntaxCode">(self, min: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, max: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, steps: </span><span class="SyntaxType">f32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Slider_setRangeValues"><span class="titletype">func</span> <span class="titlelight">Slider.</span><span class="titlestrong">setRangeValues</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\slider.swg#L494" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set left and right values, for a range. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setRangeValues</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, left: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, right: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Slider_setValue"><span class="titletype">func</span> <span class="titlelight">Slider.</span><span class="titlestrong">setValue</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\slider.swg#L465" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setValue</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, f: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Slider_setValueLeft"><span class="titletype">func</span> <span class="titlelight">Slider.</span><span class="titlestrong">setValueLeft</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\slider.swg#L468" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the left value (in case of a range). </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setValueLeft</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, f: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Slider_setValueRight"><span class="titletype">func</span> <span class="titlelight">Slider.</span><span class="titlestrong">setValueRight</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\slider.swg#L476" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the right value (in case of a range). </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setValueRight</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, f: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_SliderCtrl"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">SliderCtrl</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\sliderctrl.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> frameWnd</td>
<td><a href="#Gui_FrameWnd">Gui.FrameWnd</a></td>
<td></td>
</tr>
<tr>
<td>labelSize</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>editSize</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>sigChanged</td>
<td>Gui.SigArray'(closure(*Gui.SliderCtrl))</td>
<td></td>
</tr>
<tr>
<td>label</td>
<td>*Gui.Label</td>
<td></td>
</tr>
<tr>
<td>slider</td>
<td>*Gui.Slider</td>
<td></td>
</tr>
<tr>
<td>edit</td>
<td>*Gui.EditBox</td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_SliderCtrl_create">create(*Wnd, string, const ref Rectangle, WndId)</a></td>
<td>Create the popup list, but do not display it. </td>
</tr>
<tr>
<td><a href="#Gui_SliderCtrl_setMinMaxF32">setMinMaxF32(self, f32, f32)</a></td>
<td>Set a f32 range. </td>
</tr>
<tr>
<td><a href="#Gui_SliderCtrl_setMinMaxS64">setMinMaxS64(self, s64, s64)</a></td>
<td>Set a f64 range. </td>
</tr>
<tr>
<td><a href="#Gui_SliderCtrl_setMinMaxU64">setMinMaxU64(self, u64, u64)</a></td>
<td>Set a u64 range. </td>
</tr>
<tr>
<td><a href="#Gui_SliderCtrl_setValue">setValue(self, f32)</a></td>
<td>Set the slider current value. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_SliderCtrl_IWnd_onResizeEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onResizeEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\sliderctrl.swg#L19" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onResizeEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_ResizeEvent">ResizeEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_SliderCtrl_create"><span class="titletype">func</span> <span class="titlelight">SliderCtrl.</span><span class="titlestrong">create</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\sliderctrl.swg#L37" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Create the popup list, but do not display it. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">create</span><span class="SyntaxCode">(parent: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode">, name: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, position: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode"> = {}, id: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_SliderCtrl">SliderCtrl</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_SliderCtrl_setMinMaxF32"><span class="titletype">func</span> <span class="titlelight">SliderCtrl.</span><span class="titlestrong">setMinMaxF32</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\sliderctrl.swg#L96" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set a f32 range. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setMinMaxF32</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, min: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, max: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_SliderCtrl_setMinMaxS64"><span class="titletype">func</span> <span class="titlelight">SliderCtrl.</span><span class="titlestrong">setMinMaxS64</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\sliderctrl.swg#L111" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set a f64 range. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setMinMaxS64</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, min: </span><span class="SyntaxType">s64</span><span class="SyntaxCode">, max: </span><span class="SyntaxType">s64</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_SliderCtrl_setMinMaxU64"><span class="titletype">func</span> <span class="titlelight">SliderCtrl.</span><span class="titlestrong">setMinMaxU64</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\sliderctrl.swg#L103" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set a u64 range. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setMinMaxU64</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, min: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">, max: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_SliderCtrl_setValue"><span class="titletype">func</span> <span class="titlelight">SliderCtrl.</span><span class="titlestrong">setValue</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\sliderctrl.swg#L119" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the slider current value. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setValue</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, value: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_SliderFlags"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">SliderFlags</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\slider.swg#L5" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>Zero</td>
<td></td>
</tr>
<tr>
<td>Range</td>
<td></td>
</tr>
<tr>
<td>HilightBar</td>
<td></td>
</tr>
<tr>
<td>Vertical</td>
<td></td>
</tr>
<tr>
<td>Integer</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_SplitterCtrl"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">SplitterCtrl</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\splitterctrl.swg#L23" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> frameWnd</td>
<td><a href="#Gui_FrameWnd">Gui.FrameWnd</a></td>
<td></td>
</tr>
<tr>
<td>splitterFlags</td>
<td><a href="#Gui_SplitterFlags">Gui.SplitterFlags</a></td>
<td></td>
</tr>
<tr>
<td>titleSize</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>correctMoving</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>hotIdx</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>moving</td>
<td>bool</td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_SplitterCtrl_addPane">addPane(self, *Wnd, f32, bool)</a></td>
<td>Add a new pane. </td>
</tr>
<tr>
<td><a href="#Gui_SplitterCtrl_create">create(*Wnd, const ref Rectangle, bool, WndId)</a></td>
<td>Creates a new SplitterCtrl instance. </td>
</tr>
<tr>
<td><a href="#Gui_SplitterCtrl_isVertical">isVertical(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_SplitterCtrl_setPaneSize">setPaneSize(self, u32, f32)</a></td>
<td>Set the pane size (size of the first view). </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_SplitterCtrl_IWnd_onMouseEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onMouseEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\splitterctrl.swg#L37" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onMouseEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_MouseEvent">MouseEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_SplitterCtrl_IWnd_onPostPaintEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onPostPaintEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\splitterctrl.swg#L152" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onPostPaintEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_PaintEvent">PaintEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_SplitterCtrl_IWnd_onResizeEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onResizeEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\splitterctrl.swg#L102" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onResizeEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_ResizeEvent">ResizeEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_SplitterCtrl_addPane"><span class="titletype">func</span> <span class="titlelight">SplitterCtrl.</span><span class="titlestrong">addPane</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\splitterctrl.swg#L403" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a new pane. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">addPane</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, pane: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode">, paneSize: </span><span class="SyntaxType">f32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">100</span><span class="SyntaxCode">, title = </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_SplitterItem">SplitterItem</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_SplitterCtrl_create"><span class="titletype">func</span> <span class="titlelight">SplitterCtrl.</span><span class="titlestrong">create</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\splitterctrl.swg#L384" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Creates a new SplitterCtrl instance. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">create</span><span class="SyntaxCode">(parent: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode">, position: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode"> = {}, vertical = </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">, id: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_SplitterCtrl">SplitterCtrl</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_SplitterCtrl_isVertical"><span class="titletype">func</span> <span class="titlelight">SplitterCtrl.</span><span class="titlestrong">isVertical</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\splitterctrl.swg#L399" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">isVertical</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_SplitterCtrl_setPaneSize"><span class="titletype">func</span> <span class="titlelight">SplitterCtrl.</span><span class="titlestrong">setPaneSize</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\splitterctrl.swg#L439" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the pane size (size of the first view). </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setPaneSize</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, idx: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">, newSize: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_SplitterFlags"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">SplitterFlags</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\splitterctrl.swg#L5" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>Zero</td>
<td></td>
</tr>
<tr>
<td>Vertical</td>
<td></td>
</tr>
<tr>
<td>HideBars</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_SplitterItem"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">SplitterItem</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\splitterctrl.swg#L12" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>view</td>
<td>*Gui.Wnd</td>
<td></td>
</tr>
<tr>
<td>size</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>minSize</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>maxSize</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>label</td>
<td>*Gui.Label</td>
<td></td>
</tr>
<tr>
<td>tryPreserveSize</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>aniBar</td>
<td><a href="#Gui_BlendColor">Gui.BlendColor</a></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_StackLayoutCtrl"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">StackLayoutCtrl</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\stacklayoutctrl.swg#L17" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> wnd</td>
<td><a href="#Gui_Wnd">Gui.Wnd</a></td>
<td></td>
</tr>
<tr>
<td>layoutKind</td>
<td><a href="#Gui_StackLayoutKind">Gui.StackLayoutKind</a></td>
<td></td>
</tr>
<tr>
<td>spacing</td>
<td>f32</td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_StackLayoutCtrl_computeLayout">computeLayout(self)</a></td>
<td>Recompute layout of all childs. </td>
</tr>
<tr>
<td><a href="#Gui_StackLayoutCtrl_create">create(*Wnd, StackLayoutKind, const ref Rectangle)</a></td>
<td>Create a layout control. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_StackLayoutCtrl_IWnd_onResizeEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onResizeEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\stacklayoutctrl.swg#L27" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onResizeEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_ResizeEvent">ResizeEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_StackLayoutCtrl_computeLayout"><span class="titletype">func</span> <span class="titlelight">StackLayoutCtrl.</span><span class="titlestrong">computeLayout</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\stacklayoutctrl.swg#L131" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Recompute layout of all childs. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">computeLayout</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_StackLayoutCtrl_create"><span class="titletype">func</span> <span class="titlelight">StackLayoutCtrl.</span><span class="titlestrong">create</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\stacklayoutctrl.swg#L123" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Create a layout control. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">create</span><span class="SyntaxCode">(parent: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode">, kind: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_StackLayoutKind">StackLayoutKind</a></span><span class="SyntaxCode"> = .</span><span class="SyntaxConstant">Top</span><span class="SyntaxCode">, position: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode"> = {})-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_StackLayoutCtrl">StackLayoutCtrl</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_StackLayoutKind"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">StackLayoutKind</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\stacklayoutctrl.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>Top</td>
<td></td>
</tr>
<tr>
<td>TopAutoSize</td>
<td></td>
</tr>
<tr>
<td>Bottom</td>
<td></td>
</tr>
<tr>
<td>Left</td>
<td></td>
</tr>
<tr>
<td>LeftAutoSize</td>
<td></td>
</tr>
<tr>
<td>Right</td>
<td></td>
</tr>
<tr>
<td>HorzCenter</td>
<td></td>
</tr>
<tr>
<td>HorzCenterVertCenter</td>
<td></td>
</tr>
<tr>
<td>VertCenter</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_StateEvent"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">StateEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\events.swg#L124" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> base</td>
<td><a href="#Gui_Event">Gui.Event</a></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Surface"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">Surface</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.swg#L11" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> native</td>
<td><a href="#Gui_NativeSurface">Gui.NativeSurface</a></td>
<td></td>
</tr>
<tr>
<td>app</td>
<td>*Gui.Application</td>
<td></td>
</tr>
<tr>
<td>wnd</td>
<td>*Gui.SurfaceWnd</td>
<td></td>
</tr>
<tr>
<td>painter</td>
<td>Pixel.Painter</td>
<td></td>
</tr>
<tr>
<td>rt</td>
<td>Pixel.RenderTarget</td>
<td></td>
</tr>
<tr>
<td>userData</td>
<td>*void</td>
<td></td>
</tr>
<tr>
<td>rc</td>
<td>Pixel.RenderingContext</td>
<td></td>
</tr>
<tr>
<td>minSize</td>
<td>Core.Math.Point</td>
<td></td>
</tr>
<tr>
<td>maxSize</td>
<td>Core.Math.Point</td>
<td></td>
</tr>
<tr>
<td>flags</td>
<td><a href="#Gui_SurfaceFlags">Gui.SurfaceFlags</a></td>
<td></td>
</tr>
<tr>
<td>dirtyRect</td>
<td>Core.Math.Rectangle</td>
<td></td>
</tr>
<tr>
<td>colorDisabled</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>isDirty</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>isHidden</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>isDisabled</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>isMinimized</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td><b>using</b> state</td>
<td><a href="#Gui_SurfaceState">Gui.SurfaceState</a></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_Surface_bringToTop">bringToTop(self)</a></td>
<td>Bring the surface at the top of the Z order. </td>
</tr>
<tr>
<td><a href="#Gui_Surface_center">center(self, *Surface)</a></td>
<td>Center this surface relative to <code class="incode">from</code>. </td>
</tr>
<tr>
<td><a href="#Gui_Surface_constaintToScreen">constaintToScreen(self)</a></td>
<td>Constraint a window to be inside the closest monitor. </td>
</tr>
<tr>
<td><a href="#Gui_Surface_create">create(*Surface, s32, s32, s32, s32, SurfaceFlags, *Surface)</a></td>
<td>Creates the surface. </td>
</tr>
<tr>
<td><a href="#Gui_Surface_destroy">destroy(self)</a></td>
<td>Destroy the surface. </td>
</tr>
<tr>
<td><a href="#Gui_Surface_disable">disable(self)</a></td>
<td>Enable the surface. </td>
</tr>
<tr>
<td><a href="#Gui_Surface_enable">enable(self)</a></td>
<td>Enable the surface. </td>
</tr>
<tr>
<td><a href="#Gui_Surface_getView">getView(self)</a></td>
<td>Returns inside view. </td>
</tr>
<tr>
<td><a href="#Gui_Surface_hide">hide(self)</a></td>
<td>Hide the surface. </td>
</tr>
<tr>
<td><a href="#Gui_Surface_invalidate">invalidate(self)</a></td>
<td>Invalidate the full surface to be painted. </td>
</tr>
<tr>
<td><a href="#Gui_Surface_invalidateRect">invalidateRect(self, const ref Rectangle)</a></td>
<td>Invalidate one part of the surface to be painted. </td>
</tr>
<tr>
<td><a href="#Gui_Surface_paint">paint(self)</a></td>
<td>Main surface paint function. </td>
</tr>
<tr>
<td><a href="#Gui_Surface_screenToSurface">screenToSurface(self, *Point)</a></td>
<td>Transform a point in screen space relative to the surface. </td>
</tr>
<tr>
<td><a href="#Gui_Surface_setIcon">setIcon(self, const ref Icon)</a></td>
<td>Set the surface icon. </td>
</tr>
<tr>
<td><a href="#Gui_Surface_setPosition">setPosition(self, const ref Rectangle)</a></td>
<td>Set the surface position. </td>
</tr>
<tr>
<td><a href="#Gui_Surface_setSurfaceFlags">setSurfaceFlags(self, SurfaceFlags, SurfaceFlags)</a></td>
<td>Change flag surfaces. </td>
</tr>
<tr>
<td><a href="#Gui_Surface_setTitle">setTitle(self, string)</a></td>
<td>Set the surface title. </td>
</tr>
<tr>
<td><a href="#Gui_Surface_setView">setView(self, *Wnd)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Surface_show">show(self)</a></td>
<td>Show the surface. </td>
</tr>
<tr>
<td><a href="#Gui_Surface_showMaximized">showMaximized(self)</a></td>
<td>Show the surface as maximized. </td>
</tr>
<tr>
<td><a href="#Gui_Surface_showMinimized">showMinimized(self)</a></td>
<td>Show the surface as minimized. </td>
</tr>
<tr>
<td><a href="#Gui_Surface_showNormal">showNormal(self)</a></td>
<td>Show the surface in its default state. </td>
</tr>
<tr>
<td><a href="#Gui_Surface_surfaceToScreen">surfaceToScreen(self, *Point)</a></td>
<td>Transform a point relative to the surface in screen space. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Surface_bringToTop"><span class="titletype">func</span> <span class="titlelight">Surface.</span><span class="titlestrong">bringToTop</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.win32.swg#L240" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Bring the surface at the top of the Z order. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">bringToTop</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Surface_center"><span class="titletype">func</span> <span class="titlelight">Surface.</span><span class="titlestrong">center</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.swg#L169" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Center this surface relative to <code class="incode">from</code>. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">center</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, from: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Surface">Surface</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Surface_constaintToScreen"><span class="titletype">func</span> <span class="titlelight">Surface.</span><span class="titlestrong">constaintToScreen</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.swg#L180" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Constraint a window to be inside the closest monitor. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">constaintToScreen</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Surface_create"><span class="titletype">func</span> <span class="titlelight">Surface.</span><span class="titlestrong">create</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.swg#L97" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Creates the surface. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">create</span><span class="SyntaxCode">(surface: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Surface">Surface</a></span><span class="SyntaxCode">, x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, width: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, height: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, flags: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_SurfaceFlags">SurfaceFlags</a></span><span class="SyntaxCode">, mainSurface: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Surface">Surface</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Surface_destroy"><span class="titletype">func</span> <span class="titlelight">Surface.</span><span class="titlestrong">destroy</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.swg#L121" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Destroy the surface. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">destroy</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Surface_disable"><span class="titletype">func</span> <span class="titlelight">Surface.</span><span class="titlestrong">disable</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.win32.swg#L255" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Enable the surface. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">disable</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Surface_enable"><span class="titletype">func</span> <span class="titlelight">Surface.</span><span class="titlestrong">enable</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.win32.swg#L246" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Enable the surface. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">enable</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Surface_getView"><span class="titletype">func</span> <span class="titlelight">Surface.</span><span class="titlestrong">getView</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.swg#L128" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns inside view. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getView</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Surface_hide"><span class="titletype">func</span> <span class="titlelight">Surface.</span><span class="titlestrong">hide</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.win32.swg#L285" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Hide the surface. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">hide</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Surface_invalidate"><span class="titletype">func</span> <span class="titlelight">Surface.</span><span class="titlestrong">invalidate</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.swg#L145" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Invalidate the full surface to be painted. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">invalidate</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Surface_invalidateRect"><span class="titletype">func</span> <span class="titlelight">Surface.</span><span class="titlestrong">invalidateRect</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.swg#L151" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Invalidate one part of the surface to be painted. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">invalidateRect</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, rect: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Surface_paint"><span class="titletype">func</span> <span class="titlelight">Surface.</span><span class="titlestrong">paint</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.swg#L212" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Main surface paint function. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">paint</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Surface_screenToSurface"><span class="titletype">func</span> <span class="titlelight">Surface.</span><span class="titlestrong">screenToSurface</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.win32.swg#L326" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Transform a point in screen space relative to the surface. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">screenToSurface</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, pt: *</span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Point</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Surface_setIcon"><span class="titletype">func</span> <span class="titlelight">Surface.</span><span class="titlestrong">setIcon</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.swg#L131" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the surface icon. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setIcon</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, icon: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Icon">Icon</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Surface_setPosition"><span class="titletype">func</span> <span class="titlelight">Surface.</span><span class="titlestrong">setPosition</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.win32.swg#L222" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the surface position. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setPosition</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, pos: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Surface_setSurfaceFlags"><span class="titletype">func</span> <span class="titlelight">Surface.</span><span class="titlestrong">setSurfaceFlags</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.swg#L58" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Change flag surfaces. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setSurfaceFlags</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, add = </span><span class="SyntaxConstant"><a href="#Gui_SurfaceFlags">SurfaceFlags</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Zero</span><span class="SyntaxCode">, remove = </span><span class="SyntaxConstant"><a href="#Gui_SurfaceFlags">SurfaceFlags</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Zero</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Surface_setTitle"><span class="titletype">func</span> <span class="titlelight">Surface.</span><span class="titlestrong">setTitle</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.swg#L138" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the surface title. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setTitle</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, name: </span><span class="SyntaxType">string</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Surface_setView"><span class="titletype">func</span> <span class="titlelight">Surface.</span><span class="titlestrong">setView</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.swg#L290" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setView</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, view: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Surface_show"><span class="titletype">func</span> <span class="titlelight">Surface.</span><span class="titlestrong">show</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.win32.swg#L264" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Show the surface. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">show</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Surface_showMaximized"><span class="titletype">func</span> <span class="titlelight">Surface.</span><span class="titlestrong">showMaximized</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.win32.swg#L296" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Show the surface as maximized. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">showMaximized</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Surface_showMinimized"><span class="titletype">func</span> <span class="titlelight">Surface.</span><span class="titlestrong">showMinimized</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.win32.swg#L317" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Show the surface as minimized. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">showMinimized</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Surface_showNormal"><span class="titletype">func</span> <span class="titlelight">Surface.</span><span class="titlestrong">showNormal</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.win32.swg#L307" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Show the surface in its default state. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">showNormal</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Surface_surfaceToScreen"><span class="titletype">func</span> <span class="titlelight">Surface.</span><span class="titlestrong">surfaceToScreen</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.win32.swg#L335" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Transform a point relative to the surface in screen space. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">surfaceToScreen</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, pt: *</span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Point</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_SurfaceFlags"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">SurfaceFlags</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.swg#L37" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>Zero</td>
<td></td>
</tr>
<tr>
<td>BigCaption</td>
<td></td>
</tr>
<tr>
<td>SmallCaption</td>
<td></td>
</tr>
<tr>
<td>MinimizeBtn</td>
<td></td>
</tr>
<tr>
<td>MaximizeBtn</td>
<td></td>
</tr>
<tr>
<td>CloseBtn</td>
<td></td>
</tr>
<tr>
<td>Border</td>
<td></td>
</tr>
<tr>
<td>Sizeable</td>
<td></td>
</tr>
<tr>
<td>Secondary</td>
<td></td>
</tr>
<tr>
<td>TopMost</td>
<td></td>
</tr>
<tr>
<td>OverlappedWindow</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_SurfaceState"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">SurfaceState</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>position</td>
<td>Core.Math.Rectangle</td>
<td></td>
</tr>
<tr>
<td>positionNotMaximized</td>
<td>Core.Math.Rectangle</td>
<td></td>
</tr>
<tr>
<td>isMaximized</td>
<td>bool</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_SurfaceWnd"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">SurfaceWnd</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\surfacewnd.swg#L13" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> base</td>
<td><a href="#Gui_FrameWnd">Gui.FrameWnd</a></td>
<td></td>
</tr>
<tr>
<td>icon</td>
<td><a href="#Gui_Icon">Gui.Icon</a></td>
<td></td>
</tr>
<tr>
<td>minimizeBtn</td>
<td>*Gui.IconButton</td>
<td></td>
</tr>
<tr>
<td>maximizeBtn</td>
<td>*Gui.IconButton</td>
<td></td>
</tr>
<tr>
<td>closeBtn</td>
<td>*Gui.IconButton</td>
<td></td>
</tr>
<tr>
<td>movingSurface</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>sizingBorder</td>
<td><a href="#Gui_SizingBorder">Gui.SizingBorder</a></td>
<td></td>
</tr>
<tr>
<td>cursorSet</td>
<td>bool</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_SysCommandEvent"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">SysCommandEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\events.swg#L189" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> base</td>
<td><a href="#Gui_Event">Gui.Event</a></td>
<td></td>
</tr>
<tr>
<td>sysKind</td>
<td>Gui.SysCommandEvent.Kind</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_SysUserEvent"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">SysUserEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\events.swg#L181" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> base</td>
<td><a href="#Gui_Event">Gui.Event</a></td>
<td></td>
</tr>
<tr>
<td>userMsg</td>
<td>u64</td>
<td></td>
</tr>
<tr>
<td>param0</td>
<td>u64</td>
<td></td>
</tr>
<tr>
<td>param1</td>
<td>u64</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Tab"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">Tab</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\tab.swg#L52" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> wnd</td>
<td><a href="#Gui_Wnd">Gui.Wnd</a></td>
<td></td>
</tr>
<tr>
<td>barForm</td>
<td><a href="#Gui_TabBarForm">Gui.TabBarForm</a></td>
<td></td>
</tr>
<tr>
<td>viewForm</td>
<td><a href="#Gui_TabViewForm">Gui.TabViewForm</a></td>
<td></td>
</tr>
<tr>
<td>barLayout</td>
<td><a href="#Gui_TabBarLayout">Gui.TabBarLayout</a></td>
<td></td>
</tr>
<tr>
<td>sigSelChanged</td>
<td>Gui.SigArray'(closure(*Gui.Tab))</td>
<td></td>
</tr>
<tr>
<td>buttonPopup</td>
<td>*Gui.IconButton</td>
<td></td>
</tr>
<tr>
<td>views</td>
<td>Core.ArrayPtr'(Gui.TabItem)</td>
<td></td>
</tr>
<tr>
<td>popup</td>
<td>*Gui.PopupListCtrl</td>
<td></td>
</tr>
<tr>
<td>selectedIdx</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>firstVisibleIdx</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>hotIdx</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>barHeight</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>mostRightPos</td>
<td>f32</td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_Tab_addItem">addItem(self, string, const ref Icon, *Wnd)</a></td>
<td>Add a new view. </td>
</tr>
<tr>
<td><a href="#Gui_Tab_create">create(*Wnd, const ref Rectangle, WndId)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Tab_select">select(self, u32)</a></td>
<td>Select one tab at the given index. </td>
</tr>
<tr>
<td><a href="#Gui_Tab_setForm">setForm(self, TabForm)</a></td>
<td>Set the tab form. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Tab_IWnd_onMouseEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onMouseEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\tab.swg#L243" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onMouseEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_MouseEvent">MouseEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Tab_IWnd_onPaintEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onPaintEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\tab.swg#L165" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onPaintEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_PaintEvent">PaintEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Tab_IWnd_onResizeEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onResizeEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\tab.swg#L225" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onResizeEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_ResizeEvent">ResizeEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Tab_IWnd_paintItem"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">paintItem</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\tab.swg#L78" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">paintItem</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, painter: *</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Painter</span><span class="SyntaxCode">, v: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_TabItem">TabItem</a></span><span class="SyntaxCode">, idx: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Tab_addItem"><span class="titletype">func</span> <span class="titlelight">Tab.</span><span class="titlestrong">addItem</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\tab.swg#L473" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a new view. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">addItem</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, tabName: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, icon: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Icon">Icon</a></span><span class="SyntaxCode">, view: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Tab_create"><span class="titletype">func</span> <span class="titlelight">Tab.</span><span class="titlestrong">create</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\tab.swg#L435" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">create</span><span class="SyntaxCode">(parent: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode">, position: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode"> = {}, id: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Tab">Tab</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Tab_select"><span class="titletype">func</span> <span class="titlelight">Tab.</span><span class="titlestrong">select</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\tab.swg#L494" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Select one tab at the given index. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">select</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, idx: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Tab_setForm"><span class="titletype">func</span> <span class="titlelight">Tab.</span><span class="titlestrong">setForm</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\tab.swg#L445" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the tab form. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setForm</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, tabForm: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_TabForm">TabForm</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_TabBarForm"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">TabBarForm</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\tab.swg#L11" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>Transparent</td>
<td></td>
</tr>
<tr>
<td>Flat</td>
<td></td>
</tr>
<tr>
<td>FlatRound</td>
<td></td>
</tr>
<tr>
<td>Round</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_TabBarLayout"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">TabBarLayout</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\tab.swg#L35" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>Divide</td>
<td></td>
</tr>
<tr>
<td>Fixed</td>
<td></td>
</tr>
<tr>
<td>Auto</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_TabForm"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">TabForm</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\tab.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>Flat</td>
<td></td>
</tr>
<tr>
<td>Round</td>
<td></td>
</tr>
<tr>
<td>Button</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_TabItem"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">TabItem</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\tab.swg#L42" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>pos</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>width</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>view</td>
<td>*Gui.Wnd</td>
<td></td>
</tr>
<tr>
<td>aniImg</td>
<td><a href="#Gui_BlendColor">Gui.BlendColor</a></td>
<td></td>
</tr>
<tr>
<td>aniText</td>
<td><a href="#Gui_BlendColor">Gui.BlendColor</a></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_TabItemForm"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">TabItemForm</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\tab.swg#L20" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>Transparent</td>
<td></td>
</tr>
<tr>
<td>Flat</td>
<td></td>
</tr>
<tr>
<td>Round</td>
<td></td>
</tr>
<tr>
<td>Button</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_TabViewForm"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">TabViewForm</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\tab.swg#L28" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>None</td>
<td></td>
</tr>
<tr>
<td>Flat</td>
<td></td>
</tr>
<tr>
<td>Round</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Theme"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">Theme</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\theme.swg#L52" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>intialized</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>res</td>
<td><a href="#Gui_ThemeResources">Gui.ThemeResources</a></td>
<td></td>
</tr>
<tr>
<td>rects</td>
<td><a href="#Gui_ThemeImageRects">Gui.ThemeImageRects</a></td>
<td></td>
</tr>
<tr>
<td>metrics</td>
<td><a href="#Gui_ThemeMetrics">Gui.ThemeMetrics</a></td>
<td></td>
</tr>
<tr>
<td>colors</td>
<td><a href="#Gui_ThemeColors">Gui.ThemeColors</a></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_Theme_createDefaultFont">createDefaultFont(self, u32, FontFamilyStyle)</a></td>
<td>Create a default font with the given size. </td>
</tr>
<tr>
<td><a href="#Gui_Theme_createDefaultFontFS">createDefaultFontFS(self, u32, FontFamilyStyle)</a></td>
<td>Create a default font (fixed size) with the given size. </td>
</tr>
<tr>
<td><a href="#Gui_Theme_drawIconText">drawIconText(*Painter, const ref Rectangle, const ref Icon, *RichString, const ref RichStringFormat, f32)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Theme_drawSubRect">drawSubRect(self, *Painter, const ref Rectangle, *ThemeImageRect, const ref Color, bool, InterpolationMode)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Theme_drawSubRect">drawSubRect(*Painter, const ref Texture, const ref Rectangle, *ThemeImageRect, const ref Color, bool, InterpolationMode)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Theme_getIcon24">getIcon24(self, ThemeIcons24, f32)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Theme_getIcon64">getIcon64(self, ThemeIcons64, f32)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Theme_setTextColors">setTextColors(self, *RichStringFormat, const ref Color)</a></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Theme_createDefaultFont"><span class="titletype">func</span> <span class="titlelight">Theme.</span><span class="titlestrong">createDefaultFont</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\theme.swg#L111" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Create a default font with the given size. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">createDefaultFont</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, size: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">, style = </span><span class="SyntaxConstant">FontFamilyStyle</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Regular</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Font</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Theme_createDefaultFontFS"><span class="titletype">func</span> <span class="titlelight">Theme.</span><span class="titlestrong">createDefaultFontFS</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\theme.swg#L126" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Create a default font (fixed size) with the given size. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">createDefaultFontFS</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, size: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">, style = </span><span class="SyntaxConstant">FontFamilyStyle</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Regular</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Font</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Theme_drawIconText"><span class="titletype">func</span> <span class="titlelight">Theme.</span><span class="titlestrong">drawIconText</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\themepaint.swg#L93" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">drawIconText</span><span class="SyntaxCode">(painter: *</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Painter</span><span class="SyntaxCode">, position: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode">, icon: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Icon">Icon</a></span><span class="SyntaxCode">, name: *</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">RichString</span><span class="SyntaxCode">, stringFormat: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">RichStringFormat</span><span class="SyntaxCode">, iconMargin = </span><span class="SyntaxNumber">4</span><span class="SyntaxCode">'</span><span class="SyntaxType">f32</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Theme_drawSubRect"><span class="titletype">func</span> <span class="titlelight">Theme.</span><span class="titlestrong">drawSubRect</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\themepaint.swg#L6" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">drawSubRect</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, painter: *</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Painter</span><span class="SyntaxCode">, dstRect: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode">, subRect: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span><span class="SyntaxCode">, color: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Color</span><span class="SyntaxCode">, center = </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">, interpol = </span><span class="SyntaxConstant">InterpolationMode</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Linear</span><span class="SyntaxCode">)
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">drawSubRect</span><span class="SyntaxCode">(painter: *</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Painter</span><span class="SyntaxCode">, texture: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Texture</span><span class="SyntaxCode">, dstRect: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode">, subRect: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span><span class="SyntaxCode">, color: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Color</span><span class="SyntaxCode">, center = </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">, interpol = </span><span class="SyntaxConstant">InterpolationMode</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Linear</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Theme_getIcon24"><span class="titletype">func</span> <span class="titlelight">Theme.</span><span class="titlestrong">getIcon24</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\themeicons.swg#L42" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getIcon24</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, idx: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_ThemeIcons24">ThemeIcons24</a></span><span class="SyntaxCode">, size = </span><span class="SyntaxNumber">24</span><span class="SyntaxCode">'</span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Icon">Icon</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Theme_getIcon64"><span class="titletype">func</span> <span class="titlelight">Theme.</span><span class="titlestrong">getIcon64</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\themeicons.swg#L52" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getIcon64</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, idx: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_ThemeIcons64">ThemeIcons64</a></span><span class="SyntaxCode">, size = </span><span class="SyntaxNumber">64</span><span class="SyntaxCode">'</span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Icon">Icon</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Theme_setTextColors"><span class="titletype">func</span> <span class="titlelight">Theme.</span><span class="titlestrong">setTextColors</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\themepaint.swg#L121" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setTextColors</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, fmt: *</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">RichStringFormat</span><span class="SyntaxCode">, mainColor: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Color</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ThemeColors"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">ThemeColors</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\themecolors.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>Transparent</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>Blue</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>LightBlue</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>Disabled</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>LighterSmall</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>LighterMedium</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>LighterBig</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>LighterVeryBig</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>White</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>Gray1</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>Gray2</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>Gray3</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>Gray4</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>Gray5</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>Gray6</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>Gray7</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>Gray8</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>richColor1</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>richColor2</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>richColor3</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>surfaceDisabled</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>hilight</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>hilightLight</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>hilightDark</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>disabled</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>transparent</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>lighterSmall</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>lighterMedium</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>lighterBig</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>lighterVeryBig</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>wnd_Bk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>wnd_Caption</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>wnd_CaptionNotActived</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>wnd_CaptionText</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>wnd_CaptionNotActivatedText</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>wnd_BtnCloseBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>wnd_BtnCloseHotBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>wnd_Shadow</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>view_Bk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>dlg_Bk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>dlg_BtnBarBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnPush_StrongText</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnPush_StrongBorder</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnPush_StrongBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnPush_Text</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnPush_Border</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnPush_Bk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnPush_PressedText</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnPush_PressedBorder</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnPush_PressedBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnPush_HotText</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnPush_HotBorder</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnPush_HotBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnPush_DisabledText</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnPush_DisabledBorder</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnPush_DisabledBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnPushFlat_DisabledText</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnPushFlat_DisabledBorder</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnPushFlat_DisabledBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnPushFlat_Text</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnPushFlat_Border</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnPushFlat_Bk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnPushFlat_PressedText</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnPushFlat_PressedBorder</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnPushFlat_PressedBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnPushFlat_HotText</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnPushFlat_HotBorder</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnPushFlat_HotBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnPushDefault_DisabledText</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnPushDefault_DisabledBorder</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnPushDefault_DisabledBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnPushDefault_Text</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnPushDefault_Border</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnPushDefault_Bk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnPushDefault_PressedText</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnPushDefault_PressedBorder</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnPushDefault_PressedBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnPushDefault_HotText</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnPushDefault_HotBorder</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnPushDefault_HotBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnIcon_Icon</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnIcon_Text</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnIcon_Bk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnIcon_DisabledIcon</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnIcon_DisabledText</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnIcon_DisabledBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnIcon_PressedIcon</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnIcon_PressedText</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnIcon_PressedBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnIcon_HotIcon</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnIcon_HotText</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnIcon_HotBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnIcon_CheckedDisabledIcon</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnIcon_CheckedDisabledText</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnIcon_CheckedPressedIcon</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnIcon_CheckedPressedText</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnIcon_CheckedHotIcon</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnIcon_CheckedHotText</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnIcon_CheckedIcon</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnIcon_CheckedText</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnIcon_CheckedDisabledBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnIcon_CheckedPressedBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnIcon_CheckedHotBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnIcon_CheckedBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnIcon_CheckedMark</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnIcon_CheckedPressedMark</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnIcon_CheckedHotMark</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnIcon_CheckedDisabledMark</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>label_Text</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>label_TextLight</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>label_Text1</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>label_Text2</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>label_Text3</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>label_DisabledText</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>label_Icon</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>label_DisabledIcon</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnCheck_Text</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnCheck_Border</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnCheck_Mark</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnCheck_Bk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnCheck_PressedText</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnCheck_PressedBorder</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnCheck_PressedMark</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnCheck_PressedBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnCheck_HotText</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnCheck_HotBorder</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnCheck_HotMark</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnCheck_HotBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnCheck_DisabledText</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnCheck_DisabledBorder</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnCheck_DisabledMark</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnCheck_DisabledBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnCheck_CheckedText</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnCheck_CheckedBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnCheck_CheckedBorder</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnCheck_CheckedMark</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>toolTip_Bk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>toolTip_Border</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>toolTip_Text</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>edit_Border</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>edit_FocusBorder</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>edit_HotBorder</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>edit_DisabledBorder</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>edit_Bk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>edit_DisabledBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>edit_ErrorBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>edit_HotBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>edit_FocusBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>edit_Note</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>edit_Text</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>edit_DisabledText</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>edit_SelectedText</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>edit_SelectedBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>edit_ErrorBorder</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>richEdit_Bk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>richEdit_TextSpecial</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>richEdit_Text</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>richEdit_SelText</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>richEdit_SelTextNoFocus</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>richEdit_SelBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>richEdit_SelBkNoFocus</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>richEdit_Caret</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>richEdit_CurLineBorder</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnToggle_DisabledBorder</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnToggle_DisabledMark</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnToggle_DisabledBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnToggle_OffBorder</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnToggle_OffMark</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnToggle_OffBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnToggle_OffHotBorder</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnToggle_OffHotMark</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnToggle_OffHotBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnToggle_OffPressedBorder</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnToggle_OffPressedMark</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnToggle_OffPressedBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnToggle_OnBorder</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnToggle_OnMark</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnToggle_OnBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnToggle_OnHotBorder</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnToggle_OnHotMark</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnToggle_OnHotBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnToggle_OnPressedBorder</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnToggle_OnPressedMark</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>btnToggle_OnPressedBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>slider_BkLeft</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>slider_BkMiddle</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>slider_BkRight</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>slider_HilightBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>slider_Mark</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>slider_HotMark</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>slider_PressedMark</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>slider_DisabledMark</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>slider_DisabledBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>slider_StepBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>slider_HilightStepBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>slider_HotStepBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>tabItem_DisabledText</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>tabItem_SelectedText</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>tabItem_UnSelectedText</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>tabItem_FlatSelected</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>tabItem_FlatHot</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>tabItem_RoundSelected</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>tabItem_RoundHot</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>tabItem_ButtonSelected</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>tabItem_ButtonHot</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>tabBar_FlatBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>tabBar_RoundBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>tabView_FlatBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>tabView_RoundBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>menuPopup_Bk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>menuPopup_Border</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>menuPopup_Shadow</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>menuPopup_Separator</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>menuBar_Bk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>menuBar_Border</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>menuBar_SelectedBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>popupItem_Text</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>popupItem_RightText</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>popupItem_DisabledText</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>popupItem_HotText</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>popupItem_HotRightText</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>popupItem_HotBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>scrollBar_Bk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>scrollBar_HotBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>scrollBar_DisabledBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>scrollBar_Box</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>scrollBar_HotBox</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>scrollBar_PressedBox</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>comboBox_Border</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>comboBox_HotBorder</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>comboBox_DisabledBorder</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>comboBox_SelectedBorder</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>comboBox_Bk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>comboBox_HotBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>comboBox_DisabledBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>comboBox_SelectedBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>comboBox_Text</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>comboBox_HotText</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>comboBox_DisabledText</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>comboBox_SelectedText</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>comboBoxItem_Text</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>comboBoxItem_HotBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>comboBoxItem_HotText</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>comboBoxItem_CheckMark</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>comboBoxItem_HotCheckMark</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>popupList_Separator</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>popupList_HotBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>popupList_Text</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>popupList_HotText</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>popupList_CheckMark</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>popupList_HotCheckMark</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>frameWnd_SmallShadow</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>frameWnd_Border</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>frameWnd_PopupBorder</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>frameWnd_FocusBorder</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>frameWnd_DisabledBorder</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>frameWnd_Bk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>frameWnd_DisabledBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>frameWnd_FocusBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>listItem_FocusSelectedBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>listItem_SelectedBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>listItem_HotBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>listItem_DisabledText</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>listItem_SelectedText</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>listItem_Text</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>listItem_Separator</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>listItem_AlternateBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>listItem_Line</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>headerItem_DisabledText</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>headerItem_HotText</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>headerItem_HotBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>headerItem_ClickedBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>headerItem_Text</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>headerItem_Separator</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>header_Bk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>splitView_TitleBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>splitView_Bar</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>splitView_BarHot</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>progressBar_DisabledBorder</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>progressBar_DisabledBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>progressBar_DisabledMark</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>progressBar_Border</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>progressBar_Bk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>progressBar_Mark</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>palPicker_Border</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>palPicker_HotBorder</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>colorPicker_Border</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>colorPicker_BackAlpha</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>colorPicker_Mark</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>imageRect_Fg</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>embInfo_CriticalBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>embInfo_InformationBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>propList_focusBk</td>
<td>Pixel.Color</td>
<td></td>
</tr>
<tr>
<td>propList_focusBorder</td>
<td>Pixel.Color</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ThemeIcons24"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">ThemeIcons24</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\themeicons.swg#L3" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>WndMinimize</td>
<td></td>
</tr>
<tr>
<td>WndMaximize</td>
<td></td>
</tr>
<tr>
<td>WndClose</td>
<td></td>
</tr>
<tr>
<td>Search</td>
<td></td>
</tr>
<tr>
<td>RightPopupArrow</td>
<td></td>
</tr>
<tr>
<td>BottomPopupArrow</td>
<td></td>
</tr>
<tr>
<td>SortArrowDown</td>
<td></td>
</tr>
<tr>
<td>SortArrowUp</td>
<td></td>
</tr>
<tr>
<td>SortArrowRight</td>
<td></td>
</tr>
<tr>
<td>Picker</td>
<td></td>
</tr>
<tr>
<td>Play</td>
<td></td>
</tr>
<tr>
<td>Pause</td>
<td></td>
</tr>
<tr>
<td>Stop</td>
<td></td>
</tr>
<tr>
<td>SendToBack</td>
<td></td>
</tr>
<tr>
<td>SendBackward</td>
<td></td>
</tr>
<tr>
<td>BringForward</td>
<td></td>
</tr>
<tr>
<td>BringToFront</td>
<td></td>
</tr>
<tr>
<td>Plus</td>
<td></td>
</tr>
<tr>
<td>More</td>
<td></td>
</tr>
<tr>
<td>Undo</td>
<td></td>
</tr>
<tr>
<td>Redo</td>
<td></td>
</tr>
<tr>
<td>Copy</td>
<td></td>
</tr>
<tr>
<td>Paste</td>
<td></td>
</tr>
<tr>
<td>Cut</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ThemeIcons64"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">ThemeIcons64</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\themeicons.swg#L31" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>Question</td>
<td></td>
</tr>
<tr>
<td>Warning</td>
<td></td>
</tr>
<tr>
<td>Critical</td>
<td></td>
</tr>
<tr>
<td>Information</td>
<td></td>
</tr>
<tr>
<td>HourGlass</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ThemeImageRect"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">ThemeImageRect</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\themerects.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>rect</td>
<td>Core.Math.Rectangle</td>
<td></td>
</tr>
<tr>
<td>corner</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>keepBordersRatio</td>
<td>bool</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ThemeImageRects"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">ThemeImageRects</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\themerects.swg#L11" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>surfaceWnd_Shadow</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>wnd_Border</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>wnd_Caption</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>wnd_CaptionSmall</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>dlg_BtnBar</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>btnPush_Normal</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>btnPush_NormalBorder</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>btnPush_Hot</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>btnPush_HotBorder</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>btnPush_Pressed</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>btnPush_PressedBorder</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>btnPush_Disabled</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>btnPush_DisabledBorder</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>btnIcon_SquareBk</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>btnIcon_RoundBk</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>btnIcon_RoundSquareBk</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>btnIcon_CheckedBottom</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>btnIcon_CheckedTop</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>btnIcon_CheckedLeft</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>btnIcon_CheckedRight</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>btnIcon_CheckedFull</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>btnIcon_ArrowDown</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>btnIcon_ArrowLeft</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>btnIcon_ArrowUp</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>btnIcon_ArrowRight</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>btnCheck_Normal</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>btnCheck_NormalBorder</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>btnCheck_Undefined</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>btnCheck_UndefinedBorder</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>btnCheck_UndefinedMark</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>btnCheck_Checked</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>btnCheck_CheckedBorder</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>btnCheck_CheckedMark</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>btnRadio_NormalBorder</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>btnRadio_Normal</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>btnRadio_CheckedBorder</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>btnRadio_Checked</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>btnRadio_CheckedMark</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>edit_SquareBk</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>edit_SquareBorder</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>edit_RoundBk</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>edit_RoundBorder</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>edit_FlatBk</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>edit_FlatBorder</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>btnToggle_Bk</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>btnToggle_Border</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>btnToggle_Mark</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>slider_BkLeft</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>slider_BkMiddle</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>slider_BkRight</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>slider_BkLeftVert</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>slider_BkMiddleVert</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>slider_BkRightVert</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>slider_Mark</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>slider_Step</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>tabItem_FlatSelected</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>tabItem_RoundSelected</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>tabItem_RoundHot</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>tabItem_ButtonSelected</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>tabItem_ButtonHot</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>tabBar_FlatBk</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>tabBar_FlatRoundBk</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>tabBar_RoundBk</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>tabView_FlatBk</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>tabView_RoundBk</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>menuPopup_Shadow</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>menuPopup_Bk</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>menuPopup_Border</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>menuPopup_HotBk</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>menuBar_Bk</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>menuBar_Border</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>menuBar_SelectedBk</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>menuItem_CheckedMark</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>scrollBar_BkV</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>scrollBar_BkH</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>scrollBar_BoxV</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>scrollBar_BoxH</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>scrollBar_Corner</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>frameWnd_SmallShadow</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>frameWnd_SquareBk</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>frameWnd_SquareBorder</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>frameWnd_RoundBk</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>frameWnd_RoundBorder</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>frameWnd_AnchorTop</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>frameWnd_AnchorTopBk</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>frameWnd_AnchorBottom</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>frameWnd_AnchorBottomBk</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>frameWnd_AnchorRight</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>frameWnd_AnchorRightBk</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>frameWnd_AnchorLeft</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>frameWnd_AnchorLeftBk</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>header_FlatBk</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>header_RoundBk</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>selectionBox_Bk</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>splitView_HorzBarBk</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>splitView_HorzBarHotBk</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>splitView_VertBarBk</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>splitView_VertBarHotBk</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>progressBar_Bk</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>progressBar_Border</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>progressBar_Mark</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>colorPicker_RoundPick</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>colorPicker_SquarePick</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>colorPicker_Bk</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
<tr>
<td>colorPicker_Border</td>
<td><a href="#Gui_ThemeImageRect">Gui.ThemeImageRect</a></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ThemeMetrics"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">ThemeMetrics</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\thememetrics.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>defaultFontSize</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>defaultFixedFontSize</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>surfaceWnd_ShadowSize</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>wnd_BorderSize</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>wnd_BigCaptionCY</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>wnd_SmallCaptionCY</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>wnd_CaptionMarginTitle</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>tabBar_FlatHeight</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>tabBar_RoundHeight</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>tabBar_ButtonHeight</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>btnPush_Padding</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>btnPush_Width</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>btnPush_Height</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>iconText_Margin</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>btnCheck_Size</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>btnCheck_Padding</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>dialog_Padding</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>dialog_BtnPadding</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>editBox_SquarePadding</td>
<td>Core.Math.Vector4</td>
<td></td>
</tr>
<tr>
<td>editBox_RoundPadding</td>
<td>Core.Math.Vector4</td>
<td></td>
</tr>
<tr>
<td>editBox_FlatPadding</td>
<td>Core.Math.Vector4</td>
<td></td>
</tr>
<tr>
<td>editBox_Width</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>editBox_Height</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>btnToggle_Width</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>btnToggle_Height</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>slider_Width</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>slider_Height</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>slider_BoxSize</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>slider_StepSize</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>menuPopup_Padding</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>menuPopup_SpaceLines</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>menuPopup_ShadowSize</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>menuPopup_IconTextMargin</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>menuPopup_SeparatorHeight</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>menuBar_Height</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>menuBar_SpaceBetween</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>iconBar_ButtonsPadding</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>scrollBar_NormalSize</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>scrollBar_SmallSize</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>scrollBar_BoxPadding</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>scrollBar_BoxMinSize</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>comboBox_Width</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>comboBox_Height</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>comboBox_RightMargin</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>comboBox_Padding</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>comboBox_IconTextMargin</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>popupList_Padding</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>popupList_IconTextMargin</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>popupList_SeparatorPadding</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>btnIcon_WidthArrow</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>btnIcon_HeightArrow</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>btnIcon_Padding</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>btnIcon_MarginArrow</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>btnIcon_SpacingIcon</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>btnIcon_SpacingArrowH</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>btnIcon_SpacingArrowV</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>btnIcon_SpacingTextH</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>btnIcon_SpacingTextV</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>frameWnd_SquarePadding</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>frameWnd_RoundPadding</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>frameWnd_FlatPadding</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>frameWnd_SmallShadowSize</td>
<td>Core.Math.Vector4</td>
<td></td>
</tr>
<tr>
<td>frameWnd_AnchorWidth</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>frameWnd_AnchorHeight</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>frameWnd_AnchorOverlap</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>frameWnd_AnchorRoundBorderMargin</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>frameWnd_AnchorSquareBorderMargin</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>list_Padding</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>list_ExpandSize</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>header_Height</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>header_Padding</td>
<td>Core.Math.Vector4</td>
<td></td>
</tr>
<tr>
<td>header_Separator</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>splitView_BarSize</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>progressBar_Height</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>progressBar_ClipOffset</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>progressBar_SpeedInf</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>colorPicker_RoundPick</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>colorPicker_SquarePick</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>label_Height</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>richEdit_Padding</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>toolTip_Padding</td>
<td>f32</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ThemeResources"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">ThemeResources</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\theme.swg#L34" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>imgWidgets</td>
<td>Pixel.Texture</td>
<td></td>
</tr>
<tr>
<td>icons24</td>
<td>*Gui.ImageList</td>
<td></td>
</tr>
<tr>
<td>icons64</td>
<td>*Gui.ImageList</td>
<td></td>
</tr>
<tr>
<td>spin</td>
<td>*Gui.ImageList</td>
<td></td>
</tr>
<tr>
<td>defaultTypeFaceR</td>
<td>*Pixel.TypeFace</td>
<td></td>
</tr>
<tr>
<td>defaultTypeFaceB</td>
<td>*Pixel.TypeFace</td>
<td></td>
</tr>
<tr>
<td>defaultTypeFaceI</td>
<td>*Pixel.TypeFace</td>
<td></td>
</tr>
<tr>
<td>defaultTypeFaceBI</td>
<td>*Pixel.TypeFace</td>
<td></td>
</tr>
<tr>
<td>defaultFsTypeFaceR</td>
<td>*Pixel.TypeFace</td>
<td></td>
</tr>
<tr>
<td>defaultFsTypeFaceB</td>
<td>*Pixel.TypeFace</td>
<td></td>
</tr>
<tr>
<td>defaultFsTypeFaceI</td>
<td>*Pixel.TypeFace</td>
<td></td>
</tr>
<tr>
<td>defaultFsTypeFaceBI</td>
<td>*Pixel.TypeFace</td>
<td></td>
</tr>
<tr>
<td>fontDefault</td>
<td>Pixel.FontFamily</td>
<td></td>
</tr>
<tr>
<td>fontDefaultFs</td>
<td>Pixel.FontFamily</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ThemeStyle"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">ThemeStyle</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\themestyle.swg#L10" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>refr</td>
<td>*Gui.ThemeStyleRef</td>
<td></td>
</tr>
<tr>
<td>dirty</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>styleSheetMetrics</td>
<td>Core.String</td>
<td></td>
</tr>
<tr>
<td>styleSheetColors</td>
<td>Core.String</td>
<td></td>
</tr>
<tr>
<td>font</td>
<td>Pixel.FontFamily</td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_ThemeStyle_addStyleSheetColors">addStyleSheetColors(self, string)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_ThemeStyle_addStyleSheetMetrics">addStyleSheetMetrics(self, string)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_ThemeStyle_clearFont">clearFont(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_ThemeStyle_clearStyleSheetColors">clearStyleSheetColors(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_ThemeStyle_create">create(self, *ThemeStyle)</a></td>
<td>Create a specific style based on the parent. </td>
</tr>
<tr>
<td><a href="#Gui_ThemeStyle_getFont">getFont(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_ThemeStyle_setFont">setFont(self, *Font, FontFamilyStyle)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_ThemeStyle_setFont">setFont(self, const ref FontFamily)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_ThemeStyle_setStyleSheetColors">setStyleSheetColors(self, string)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_ThemeStyle_setStyleSheetMetrics">setStyleSheetMetrics(self, string)</a></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ThemeStyle_addStyleSheetColors"><span class="titletype">func</span> <span class="titlelight">ThemeStyle.</span><span class="titlestrong">addStyleSheetColors</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\themestyle.swg#L128" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">addStyleSheetColors</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, str: </span><span class="SyntaxType">string</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ThemeStyle_addStyleSheetMetrics"><span class="titletype">func</span> <span class="titlelight">ThemeStyle.</span><span class="titlestrong">addStyleSheetMetrics</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\themestyle.swg#L141" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">addStyleSheetMetrics</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, str: </span><span class="SyntaxType">string</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ThemeStyle_clearFont"><span class="titletype">func</span> <span class="titlelight">ThemeStyle.</span><span class="titlestrong">clearFont</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\themestyle.swg#L171" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">clearFont</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ThemeStyle_clearStyleSheetColors"><span class="titletype">func</span> <span class="titlelight">ThemeStyle.</span><span class="titlestrong">clearStyleSheetColors</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\themestyle.swg#L116" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">clearStyleSheetColors</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ThemeStyle_create"><span class="titletype">func</span> <span class="titlelight">ThemeStyle.</span><span class="titlestrong">create</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\themestyle.swg#L48" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Create a specific style based on the parent. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">create</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, parent: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_ThemeStyle">ThemeStyle</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ThemeStyle_getFont"><span class="titletype">func</span> <span class="titlelight">ThemeStyle.</span><span class="titlestrong">getFont</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\themestyle.swg#L148" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getFont</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">FontFamily</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ThemeStyle_setFont"><span class="titletype">func</span> <span class="titlelight">ThemeStyle.</span><span class="titlestrong">setFont</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\themestyle.swg#L165" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setFont</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, fnt: *</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Font</span><span class="SyntaxCode">, style: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">FontFamilyStyle</span><span class="SyntaxCode">)
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setFont</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, fnt: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">FontFamily</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ThemeStyle_setStyleSheetColors"><span class="titletype">func</span> <span class="titlelight">ThemeStyle.</span><span class="titlestrong">setStyleSheetColors</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\themestyle.swg#L122" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setStyleSheetColors</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, str: </span><span class="SyntaxType">string</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ThemeStyle_setStyleSheetMetrics"><span class="titletype">func</span> <span class="titlelight">ThemeStyle.</span><span class="titlestrong">setStyleSheetMetrics</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\themestyle.swg#L135" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setStyleSheetMetrics</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, str: </span><span class="SyntaxType">string</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ThemeStyleRef"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">ThemeStyleRef</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\themestyle.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>theme</td>
<td>*Gui.Theme</td>
<td></td>
</tr>
<tr>
<td>count</td>
<td>u32</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Timer"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">Timer</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\timer.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>timeMs</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>target</td>
<td>*Gui.Wnd</td>
<td></td>
</tr>
<tr>
<td>stopwatch</td>
<td>Core.Time.Stopwatch</td>
<td></td>
</tr>
<tr>
<td>toDelete</td>
<td>bool</td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_Timer_restart">restart(self)</a></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Timer_restart"><span class="titletype">func</span> <span class="titlelight">Timer.</span><span class="titlestrong">restart</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\timer.swg#L15" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">restart</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_TimerEvent"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">TimerEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\events.swg#L107" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> base</td>
<td><a href="#Gui_Event">Gui.Event</a></td>
<td></td>
</tr>
<tr>
<td>timer</td>
<td>*Gui.Timer</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ToggleButton"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">ToggleButton</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\togglebutton.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> button</td>
<td><a href="#Gui_Button">Gui.Button</a></td>
<td></td>
</tr>
<tr>
<td>isToggled</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>sigChanged</td>
<td>func(*Gui.ToggleButton)</td>
<td></td>
</tr>
<tr>
<td>aniBk</td>
<td><a href="#Gui_BlendColor">Gui.BlendColor</a></td>
<td></td>
</tr>
<tr>
<td>aniBorder</td>
<td><a href="#Gui_BlendColor">Gui.BlendColor</a></td>
<td></td>
</tr>
<tr>
<td>aniMark</td>
<td><a href="#Gui_BlendColor">Gui.BlendColor</a></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_ToggleButton_create">create(*Wnd, const ref Rectangle, WndId)</a></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ToggleButton_IWnd_onMouseEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onMouseEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\togglebutton.swg#L74" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onMouseEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_MouseEvent">MouseEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ToggleButton_IWnd_onPaintEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onPaintEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\togglebutton.swg#L18" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onPaintEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_PaintEvent">PaintEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ToggleButton_create"><span class="titletype">func</span> <span class="titlelight">ToggleButton.</span><span class="titlestrong">create</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\togglebutton.swg#L89" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">create</span><span class="SyntaxCode">(parent: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode">, position: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode"> = {}, id: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_ToggleButton">ToggleButton</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ToolTip_hide"><span class="titletype">func</span> <span class="titlelight">ToolTip.</span><span class="titlestrong">hide</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\tooltip.swg#L40" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Hide the current tooltip, if visible. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">hide</span><span class="SyntaxCode">()</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_ToolTip_show"><span class="titletype">func</span> <span class="titlelight">ToolTip.</span><span class="titlestrong">show</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\tooltip.swg#L72" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Show a tooltip. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">show</span><span class="SyntaxCode">(owner: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode">, surfacePos: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Point</span><span class="SyntaxCode">, str: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, delay: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Time</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Duration</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">500</span><span class="SyntaxCode">'ms)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">Wnd</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L89" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>itf</td>
<td><a href="#Gui_IWnd">Gui.IWnd</a></td>
<td></td>
</tr>
<tr>
<td>type</td>
<td>const *Swag.TypeInfo</td>
<td></td>
</tr>
<tr>
<td>surface</td>
<td>*Gui.Surface</td>
<td></td>
</tr>
<tr>
<td>style</td>
<td><a href="#Gui_ThemeStyle">Gui.ThemeStyle</a></td>
<td></td>
</tr>
<tr>
<td>wndFlags</td>
<td><a href="#Gui_WndFlags">Gui.WndFlags</a></td>
<td></td>
</tr>
<tr>
<td>userData</td>
<td>*void</td>
<td></td>
</tr>
<tr>
<td>position</td>
<td>Core.Math.Rectangle</td>
<td></td>
</tr>
<tr>
<td>minSize</td>
<td>Core.Math.Point</td>
<td></td>
</tr>
<tr>
<td>maxSize</td>
<td>Core.Math.Point</td>
<td></td>
</tr>
<tr>
<td>scrollPos</td>
<td>Core.Math.Point</td>
<td></td>
</tr>
<tr>
<td>invalidatePadding</td>
<td>Core.Math.Vector4</td>
<td></td>
</tr>
<tr>
<td>parent</td>
<td>*Gui.Wnd</td>
<td></td>
</tr>
<tr>
<td>owner</td>
<td>*Gui.Wnd</td>
<td></td>
</tr>
<tr>
<td>childs</td>
<td>Core.Array'(*Gui.Wnd)</td>
<td></td>
</tr>
<tr>
<td>name</td>
<td>Pixel.RichString</td>
<td></td>
</tr>
<tr>
<td>id</td>
<td>Gui.WndId</td>
<td></td>
</tr>
<tr>
<td>cursor</td>
<td><a href="#Gui_Cursor">Gui.Cursor</a></td>
<td></td>
</tr>
<tr>
<td>toolTip</td>
<td>Core.String</td>
<td></td>
</tr>
<tr>
<td>keyShortcuts</td>
<td>Core.Array'(Gui.KeyShortcut)</td>
<td></td>
</tr>
<tr>
<td>actions</td>
<td>Core.Array'(Gui.IActionUI)</td>
<td></td>
</tr>
<tr>
<td>margin</td>
<td>Core.Math.Vector4</td>
<td></td>
</tr>
<tr>
<td>padding</td>
<td>Core.Math.Vector4</td>
<td></td>
</tr>
<tr>
<td>dockStyle</td>
<td><a href="#Gui_DockStyle">Gui.DockStyle</a></td>
<td></td>
</tr>
<tr>
<td>anchorStyle</td>
<td><a href="#Gui_AnchorStyle">Gui.AnchorStyle</a></td>
<td></td>
</tr>
<tr>
<td>focusStrategy</td>
<td><a href="#Gui_FocusStategy">Gui.FocusStategy</a></td>
<td></td>
</tr>
<tr>
<td>backgroundStyle</td>
<td><a href="#Gui_BackgroundStyle">Gui.BackgroundStyle</a></td>
<td></td>
</tr>
<tr>
<td>isAllocated</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>isPendingDestroy</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>createEventDone</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>hookOnEvent</td>
<td>Gui.HookEvent</td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_Wnd_applyLayout">applyLayout(self)</a></td>
<td>Apply the current layout (childs constraints). </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_applyLayout">applyLayout(self, const ref Point)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_bringToFront">bringToFront(self)</a></td>
<td>Force the window to be the first painted (before all siblings). </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_bringToTop">bringToTop(self)</a></td>
<td>Force the window to be the last painted (on top of siblings). </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_captureMouse">captureMouse(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_computeStyle">computeStyle(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_create">create(*Wnd, WndId, HookEvent)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_create">create(*Wnd, const ref Rectangle, WndId, HookEvent)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_create">create(*Wnd, string, const ref Rectangle, WndId, HookEvent)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_destroy">destroy(self)</a></td>
<td>Destroy the window. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_destroyNow">destroyNow(self)</a></td>
<td>Destroy the window. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_disable">disable(self)</a></td>
<td>Disable the window. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_enable">enable(self, bool)</a></td>
<td>Enable/Disable the window. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_fitPosInParent">fitPosInParent(self, f32, f32, bool, bool)</a></td>
<td>Be sure rect is fully inside the parent rectangle (if possible). </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_getApp">getApp(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_getChildById">getChildById(self, string)</a></td>
<td>Retrieve the child with the given id. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_getClientRect">getClientRect(self)</a></td>
<td>Returns the client area. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_getClientRectPadding">getClientRectPadding(self)</a></td>
<td>Returns the client area, with <code class="incode">padding</code> applied. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_getFocus">getFocus(self)</a></td>
<td>Get the window with the keyboard focus. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_getFont">getFont(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_getKeyShortcut">getKeyShortcut(self, KeyModifiers, Key)</a></td>
<td>Get the id associated with a shortcut. null if none. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_getKeyShortcutNameFor">getKeyShortcutNameFor(self, WndId)</a></td>
<td>Get the name associated with a given id shortcut. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_getMouseCapture">getMouseCapture(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_getOwner">getOwner(self)</a></td>
<td>Get the owner of the window. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_getParentById">getParentById(self, string)</a></td>
<td>Retrieve the parent with the given id. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_getRectIn">getRectIn(self, *Wnd)</a></td>
<td>Get the wnd position relative to a given parent. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_getSurfaceRect">getSurfaceRect(self)</a></td>
<td>Get the wnd position in the surface. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_getTheme">getTheme(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_getThemeColors">getThemeColors(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_getThemeMetrics">getThemeMetrics(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_getThemeRects">getThemeRects(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_getTiming">getTiming(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_getTopView">getTopView(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_getTopWnd">getTopWnd(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_getWndAt">getWndAt(self, f32, f32, bool, bool)</a></td>
<td>Get the child window at the given coordinate  Coordinate is expressed in the parent system. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_hasFocus">hasFocus(self)</a></td>
<td>Returns true if the window has the keyboard focus. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_hide">hide(self)</a></td>
<td>Hide the window. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_invalidate">invalidate(self)</a></td>
<td>Force the window to be repainted. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_invalidateRect">invalidateRect(self, const ref Rectangle)</a></td>
<td>Force o local position to be repainted. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_isEnabled">isEnabled(self)</a></td>
<td>Returns true if window, and all its parents, are enabled. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_isParentOf">isParentOf(self, *Wnd)</a></td>
<td>Returns true if <code class="incode">child</code> is in the child hieararchy of <code class="incode">self</code>. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_isVisible">isVisible(self, bool)</a></td>
<td>Returns true if window, and all its parents, are visible. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_isVisibleState">isVisibleState(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_localToSurface">localToSurface(self, const ref Point)</a></td>
<td>Convert a local coordinate to a surface coordinate. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_localToSurface">localToSurface(self, const ref Rectangle)</a></td>
<td>Convert a local coordinate to a surface coordinate. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_move">move(self, f32, f32)</a></td>
<td>Move the wnd. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_notifyEvent">notifyEvent(self, Kind)</a></td>
<td>Send a notification event. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_ownerNotifyEvent">ownerNotifyEvent(self, Kind)</a></td>
<td>Send a notification event. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_paint">paint(self, *PaintContext)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_postCommandEvent">postCommandEvent(self, WndId)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_postEvent">postEvent(self, *Event)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_postInvalidateEvent">postInvalidateEvent(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_postQuitEvent">postQuitEvent(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_postResizeEvent">postResizeEvent(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_processEvent">processEvent(self, *Event)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_registerAction">registerAction(self)</a></td>
<td>Register one new action. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_registerKeyShortcut">registerKeyShortcut(self, KeyModifiers, Key, WndId)</a></td>
<td>Register a keyboard shortcut. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_releaseMouse">releaseMouse(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_resize">resize(self, f32, f32, bool)</a></td>
<td>Resize the wnd. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_screenToSurface">screenToSurface(self, const ref Point)</a></td>
<td>Convert a screen coordinate to a surface coordinate (relative to my surface). </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_sendCommandEvent">sendCommandEvent(self, WndId)</a></td>
<td>Send the command event with the given id to the window. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_sendComputeCommandStateEvent">sendComputeCommandStateEvent(self, WndId)</a></td>
<td>Send the command state event with the given id to the window. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_sendCreateEvent">sendCreateEvent(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_sendEvent">sendEvent(self, *Event)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_sendResizeEvent">sendResizeEvent(self)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_sendStateEvent">sendStateEvent(self, EventKind)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_serializeState">serializeState(self, *Serializer)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_setAllMargins">setAllMargins(self, const ref Vector4)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_setAllMargins">setAllMargins(self, f32)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_setAllPaddings">setAllPaddings(self, const ref Vector4)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_setAllPaddings">setAllPaddings(self, f32)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_setFocus">setFocus(self)</a></td>
<td>Set the keyboard focus. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_setMargin">setMargin(self, f32, f32, f32, f32)</a></td>
<td>Set margin for childs  If a value is Swag.F32.Inf, then it won't be changed. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_setPadding">setPadding(self, f32, f32, f32, f32)</a></td>
<td>Set padding for childs  If a value is Swag.F32.Inf, then it won't be changed. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_setParent">setParent(self, *Wnd)</a></td>
<td>Set window parent. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_setPosition">setPosition(self, const ref Rectangle, bool)</a></td>
<td>Move and size the wnd. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_setPosition">setPosition(self, f32, f32, f32, f32, bool)</a></td>
<td>Move and size the wnd. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_show">show(self, bool)</a></td>
<td>Show the window. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_surfaceToLocal">surfaceToLocal(self, const ref Point)</a></td>
<td>Convert a surface coordinate to a local coordinate (relative to me). </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_updateCommandState">updateCommandState(self)</a></td>
<td>Main function to update command state of various windows/widgets. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_IWnd_onApplyStateEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onApplyStateEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L205" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onApplyStateEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_CommandStateEvent">CommandStateEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_IWnd_onCommandEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onCommandEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L203" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onCommandEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_CommandEvent">CommandEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_IWnd_onComputeStateEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onComputeStateEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L204" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onComputeStateEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_CommandStateEvent">CommandStateEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_IWnd_onCreateEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onCreateEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L185" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onCreateEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_CreateEvent">CreateEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_IWnd_onDestroyEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onDestroyEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L186" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onDestroyEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_DestroyEvent">DestroyEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_IWnd_onEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L134" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Event">Event</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_IWnd_onFocusEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onFocusEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L200" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onFocusEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_FocusEvent">FocusEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_IWnd_onFrameEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onFrameEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L196" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onFrameEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_FrameEvent">FrameEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_IWnd_onHookEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onHookEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L194" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onHookEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Event">Event</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_IWnd_onKeyEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onKeyEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L201" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onKeyEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_KeyEvent">KeyEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_IWnd_onMouseEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onMouseEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L202" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onMouseEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_MouseEvent">MouseEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_IWnd_onNotifyEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onNotifyEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L206" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onNotifyEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_NotifyEvent">NotifyEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_IWnd_onPaintEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onPaintEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L190" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onPaintEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_PaintEvent">PaintEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_IWnd_onPostPaintEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onPostPaintEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L191" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onPostPaintEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_PaintEvent">PaintEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_IWnd_onPrePaintEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onPrePaintEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L189" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onPrePaintEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_PaintEvent">PaintEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_IWnd_onResizeEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onResizeEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L188" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onResizeEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_ResizeEvent">ResizeEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_IWnd_onSerializeStateEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onSerializeStateEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L195" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onSerializeStateEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_SerializeStateEvent">SerializeStateEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_IWnd_onSetThemeEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onSetThemeEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L192" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onSetThemeEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_SetThemeEvent">SetThemeEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_IWnd_onStateEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onStateEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L187" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onStateEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_StateEvent">StateEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_IWnd_onSysCommandEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onSysCommandEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L198" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onSysCommandEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_SysCommandEvent">SysCommandEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_IWnd_onSysUserEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onSysUserEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L199" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onSysUserEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_SysUserEvent">SysUserEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_IWnd_onTimerEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onTimerEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L193" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onTimerEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_TimerEvent">TimerEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_applyLayout"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">applyLayout</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L784" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Apply the current layout (childs constraints). </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">applyLayout</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">applyLayout</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, oldSize: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Point</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_bringToFront"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">bringToFront</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L691" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Force the window to be the first painted (before all siblings). </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">bringToFront</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_bringToTop"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">bringToTop</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L705" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Force the window to be the last painted (on top of siblings). </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">bringToTop</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_captureMouse"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">captureMouse</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L980" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">captureMouse</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_computeStyle"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">computeStyle</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L432" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">computeStyle</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_create"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">create</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L229" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">T</span><span class="SyntaxCode">) </span><span class="SyntaxFunction">create</span><span class="SyntaxCode">(parent: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">, id: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">, hook: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HookEvent</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">T</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">T</span><span class="SyntaxCode">) </span><span class="SyntaxFunction">create</span><span class="SyntaxCode">(parent: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode">, position: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode">, id: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">, hook: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HookEvent</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">T</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">T</span><span class="SyntaxCode">) </span><span class="SyntaxFunction">create</span><span class="SyntaxCode">(parent: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode">, name: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, position: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode">, id: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">, hook: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HookEvent</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">T</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_destroy"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">destroy</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L270" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Destroy the window. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">destroy</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_destroyNow"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">destroyNow</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L263" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Destroy the window. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">destroyNow</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_disable"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">disable</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L391" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Disable the window. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">disable</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_enable"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">enable</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L368" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Enable/Disable the window. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">enable</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, state = </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_fitPosInParent"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">fitPosInParent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L548" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Be sure rect is fully inside the parent rectangle (if possible). </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">fitPosInParent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, marginW: </span><span class="SyntaxType">f32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">, marginH: </span><span class="SyntaxType">f32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">, simX = </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">, simY = </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_getApp"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">getApp</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L217" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getApp</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_getChildById"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">getChildById</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L948" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Retrieve the child with the given id. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getChildById</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, childId: </span><span class="SyntaxType">string</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_getClientRect"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">getClientRect</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L599" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the client area. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getClientRect</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_getClientRectPadding"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">getClientRectPadding</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L605" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the client area, with <code class="incode">padding</code> applied. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getClientRectPadding</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_getFocus"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">getFocus</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L1029" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the window with the keyboard focus. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getFocus</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_getFont"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">getFont</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L225" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getFont</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_getKeyShortcut"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">getKeyShortcut</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L1053" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the id associated with a shortcut. null if none. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getKeyShortcut</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, mdf: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Input</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">KeyModifiers</span><span class="SyntaxCode">, key: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Input</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Key</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_getKeyShortcutNameFor"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">getKeyShortcutNameFor</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L1067" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the name associated with a given id shortcut. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getKeyShortcutNameFor</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, wndId: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">String</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_getMouseCapture"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">getMouseCapture</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L990" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getMouseCapture</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_getOwner"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">getOwner</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L333" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the owner of the window. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getOwner</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_getParentById"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">getParentById</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L963" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Retrieve the parent with the given id. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getParentById</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, parentId: </span><span class="SyntaxType">string</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_getRectIn"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">getRectIn</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L631" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the wnd position relative to a given parent. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getRectIn</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, in: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_getSurfaceRect"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">getSurfaceRect</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L614" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the wnd position in the surface. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getSurfaceRect</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_getTheme"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">getTheme</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L220" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getTheme</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_getThemeColors"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">getThemeColors</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L222" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getThemeColors</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_getThemeMetrics"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">getThemeMetrics</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L221" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getThemeMetrics</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_getThemeRects"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">getThemeRects</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L223" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getThemeRects</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_getTiming"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">getTiming</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L224" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getTiming</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_getTopView"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">getTopView</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L218" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getTopView</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_getTopWnd"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">getTopWnd</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L219" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getTopWnd</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_getWndAt"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">getWndAt</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L903" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the child window at the given coordinate  Coordinate is expressed in the parent system. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getWndAt</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, x: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, getDisabled = </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">, getHidden = </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_hasFocus"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">hasFocus</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L1035" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns true if the window has the keyboard focus. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">hasFocus</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_hide"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">hide</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L358" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Hide the window. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">hide</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_invalidate"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">invalidate</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L528" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Force the window to be repainted. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">invalidate</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_invalidateRect"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">invalidateRect</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L538" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Force o local position to be repainted. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">invalidateRect</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, rect: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_isEnabled"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">isEnabled</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L415" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns true if window, and all its parents, are enabled. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">isEnabled</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxType">bool</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_isParentOf"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">isParentOf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L932" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns true if <code class="incode">child</code> is in the child hieararchy of <code class="incode">self</code>. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">isParentOf</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, child: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">bool</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_isVisible"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">isVisible</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L397" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns true if window, and all its parents, are visible. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">isVisible</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, onlyMe = </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">bool</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_isVisibleState"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">isVisibleState</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L412" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">isVisibleState</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxType">bool</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_localToSurface"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">localToSurface</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L585" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Convert a local coordinate to a surface coordinate. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">localToSurface</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, pos: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Point</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Point</span><span class="SyntaxCode"></code>
</pre>
<p>Convert a local coordinate to a surface coordinate. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">localToSurface</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, pos: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_move"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">move</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L742" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Move the wnd. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">move</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, x: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_notifyEvent"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">notifyEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L1163" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Send a notification event. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">notifyEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, kind: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_NotifyEvent">NotifyEvent</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Kind</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_ownerNotifyEvent"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">ownerNotifyEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L1171" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Send a notification event. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">ownerNotifyEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, kind: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_NotifyEvent">NotifyEvent</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Kind</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_paint"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">paint</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L438" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">paint</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, bc: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_PaintContext">PaintContext</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_postCommandEvent"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">postCommandEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L1143" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">postCommandEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, cmdId: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_postEvent"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">postEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L1093" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">postEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Event">Event</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_postInvalidateEvent"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">postInvalidateEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L1099" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">postInvalidateEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_postQuitEvent"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">postQuitEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L1088" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">postQuitEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_postResizeEvent"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">postResizeEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L1126" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">postResizeEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_processEvent"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">processEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L1153" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">processEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Event">Event</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_registerAction"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">registerAction</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L1253" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Register one new action. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">T</span><span class="SyntaxCode">) </span><span class="SyntaxFunction">registerAction</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_registerKeyShortcut"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">registerKeyShortcut</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L1038" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Register a keyboard shortcut. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">registerKeyShortcut</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, mdf: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Input</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">KeyModifiers</span><span class="SyntaxCode">, key: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Input</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Key</span><span class="SyntaxCode">, wndId: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_releaseMouse"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">releaseMouse</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L985" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">releaseMouse</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_resize"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">resize</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L766" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Resize the wnd. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">resize</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, width: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, height: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, computeLayout = </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_screenToSurface"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">screenToSurface</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L572" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Convert a screen coordinate to a surface coordinate (relative to my surface). </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">screenToSurface</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, pos: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Point</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Point</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_sendCommandEvent"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">sendCommandEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L1228" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Send the command event with the given id to the window. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">sendCommandEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, cmdId: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode">)</code>
</pre>
<p> The command will be updated first, and if it is not disabled, it will be sent. </p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_sendComputeCommandStateEvent"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">sendComputeCommandStateEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L1200" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Send the command state event with the given id to the window. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">sendComputeCommandStateEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, cmdId: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WndId</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_CommandStateEvent">CommandStateEvent</a></span><span class="SyntaxCode"></code>
</pre>
<p> Returns the updated command state. </p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_sendCreateEvent"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">sendCreateEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L1134" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">sendCreateEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_sendEvent"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">sendEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L1106" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">sendEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Event">Event</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_sendResizeEvent"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">sendResizeEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L1118" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">sendResizeEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_sendStateEvent"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">sendStateEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L1112" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">sendStateEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, kind: </span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_EventKind">EventKind</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_serializeState"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">serializeState</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L1301" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">serializeState</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, ser: *</span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Serialization</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Serializer</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_setAllMargins"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">setAllMargins</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L666" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setAllMargins</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, value: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Vector4</span><span class="SyntaxCode">)
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setAllMargins</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, value: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_setAllPaddings"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">setAllPaddings</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L688" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setAllPaddings</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, value: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Vector4</span><span class="SyntaxCode">)
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setAllPaddings</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, value: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_setFocus"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">setFocus</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L1000" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the keyboard focus. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setFocus</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_setMargin"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">setMargin</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L647" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set margin for childs  If a value is Swag.F32.Inf, then it won't be changed. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setMargin</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, left: </span><span class="SyntaxType">f32</span><span class="SyntaxCode"> = </span><span class="SyntaxConstant">Swag</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">F32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Inf</span><span class="SyntaxCode">, top: </span><span class="SyntaxType">f32</span><span class="SyntaxCode"> = </span><span class="SyntaxConstant">Swag</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">F32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Inf</span><span class="SyntaxCode">, right: </span><span class="SyntaxType">f32</span><span class="SyntaxCode"> = </span><span class="SyntaxConstant">Swag</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">F32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Inf</span><span class="SyntaxCode">, bottom: </span><span class="SyntaxType">f32</span><span class="SyntaxCode"> = </span><span class="SyntaxConstant">Swag</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">F32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Inf</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_setPadding"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">setPadding</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L670" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set padding for childs  If a value is Swag.F32.Inf, then it won't be changed. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setPadding</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, left: </span><span class="SyntaxType">f32</span><span class="SyntaxCode"> = </span><span class="SyntaxConstant">Swag</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">F32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Inf</span><span class="SyntaxCode">, top: </span><span class="SyntaxType">f32</span><span class="SyntaxCode"> = </span><span class="SyntaxConstant">Swag</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">F32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Inf</span><span class="SyntaxCode">, right: </span><span class="SyntaxType">f32</span><span class="SyntaxCode"> = </span><span class="SyntaxConstant">Swag</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">F32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Inf</span><span class="SyntaxCode">, bottom: </span><span class="SyntaxType">f32</span><span class="SyntaxCode"> = </span><span class="SyntaxConstant">Swag</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">F32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Inf</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_setParent"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">setParent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L297" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set window parent. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setParent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, who: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_setPosition"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">setPosition</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L728" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Move and size the wnd. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setPosition</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, rect: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode">, computeLayout = </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">)</code>
</pre>
<p>Move and size the wnd. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setPosition</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, x: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, w: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, h: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, computeLayout = </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_show"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">show</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L343" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Show the window. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">show</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, b = </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_surfaceToLocal"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">surfaceToLocal</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L578" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Convert a surface coordinate to a local coordinate (relative to me). </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">surfaceToLocal</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, pos: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Point</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Point</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_Wnd_updateCommandState"><span class="titletype">func</span> <span class="titlelight">Wnd.</span><span class="titlestrong">updateCommandState</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L1184" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Main function to update command state of various windows/widgets. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">updateCommandState</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p> This will update the state of this window, and all of its childs </p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_WndFlags"><span class="titletype">enum</span> <span class="titlelight">Gui.</span><span class="titlestrong">WndFlags</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L77" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>Zero</td>
<td></td>
</tr>
<tr>
<td>NoScroll</td>
<td></td>
</tr>
<tr>
<td>ClipChildren</td>
<td></td>
</tr>
<tr>
<td>Disabled</td>
<td></td>
</tr>
<tr>
<td>Hidden</td>
<td></td>
</tr>
<tr>
<td>PreChildsPaint</td>
<td></td>
</tr>
<tr>
<td>PostChildsPaint</td>
<td></td>
</tr>
<tr>
<td>TopMost</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_WrapLayoutCtrl"><span class="titletype">struct</span> <span class="titlelight">Gui.</span><span class="titlestrong">WrapLayoutCtrl</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\wraplayoutctrl.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> wnd</td>
<td><a href="#Gui_Wnd">Gui.Wnd</a></td>
<td></td>
</tr>
<tr>
<td>spacing</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>wrapWidth</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>wrapHeight</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>resultHeight</td>
<td>f32</td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Gui_WrapLayoutCtrl_computeLayout">computeLayout(self)</a></td>
<td>Recompute layout of all childs. </td>
</tr>
<tr>
<td><a href="#Gui_WrapLayoutCtrl_create">create(*Wnd, const ref Rectangle)</a></td>
<td>Create a layout control. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_WrapLayoutCtrl_IWnd_onResizeEvent"><span class="titletype">func</span> <span class="titlelight">IWnd.</span><span class="titlestrong">onResizeEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\wraplayoutctrl.swg#L16" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">onResizeEvent</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, evt: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_ResizeEvent">ResizeEvent</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_WrapLayoutCtrl_computeLayout"><span class="titletype">func</span> <span class="titlelight">WrapLayoutCtrl.</span><span class="titlestrong">computeLayout</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\wraplayoutctrl.swg#L57" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Recompute layout of all childs. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">computeLayout</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gui_WrapLayoutCtrl_create"><span class="titletype">func</span> <span class="titlelight">WrapLayoutCtrl.</span><span class="titlestrong">create</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\wraplayoutctrl.swg#L50" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Create a layout control. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">create</span><span class="SyntaxCode">(parent: *</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_Wnd">Wnd</a></span><span class="SyntaxCode">, position: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode"> = {})-&gt;*</span><span class="SyntaxConstant">Gui</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gui_WrapLayoutCtrl">WrapLayoutCtrl</a></span><span class="SyntaxCode"></code>
</pre>
</div>
</div>
</div>
</body>
</html>
