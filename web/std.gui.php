<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta http-equiv="X-UA-Compatible" content="IE=edge">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<?php include('common/start-head.php'); ?><title>Module gui</title>
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
        
        .left a     { text-decoration: none; }
        .left ul    { list-style-type: none; margin-left: -20px; }
        .left h3    { background-color: Black; color: White; padding: 6px; }
        .right h1   { margin-top: 50px; margin-bottom: 50px; }
        .right h2   { margin-top: 35px; }
        
        .blockquote-default {
            border-radius:      5px;
            border:             1px solid Orange;
            border-left:        6px solid Orange;
            background-color:   LightYellow;
            margin:             20px;
            margin-left:        20px;
            margin-right:       20px;
            padding:            10px;
        }
        .blockquote-note {
            border-radius:      5px;
            border:             1px solid #ADCEDD;
            background-color:   #CDEEFD;
            margin:             20px;
            margin-left:        20px;
            margin-right:       20px;
            padding:            10px;
        }
        .blockquote-tip {
            border-radius:      5px;
            border:             1px solid #BCCFBC;
            background-color:   #DCEFDC;
            margin:             20px;
            margin-left:        20px;
            margin-right:       20px;
            padding:            10px;
        }
        .blockquote-warning {
            border-radius:      5px;
            border:             1px solid #DFBDB3;
            background-color:   #FFDDD3;
            margin:             20px;
            margin-left:        20px;
            margin-right:       20px;
            padding:            10px;
        }
        .blockquote-attention {
            border-radius:      5px;
            border:             1px solid #DDBAB8;
            background-color:   #FDDAD8;
            margin:             20px;
            margin-left:        20px;
            margin-right:       20px;
            padding:            10px;
        }
        .blockquote-default     p:first-child { margin-top: 0px; }
        .blockquote-default     p:last-child  { margin-bottom: 0px; }
        .blockquote-note        p:last-child  { margin-bottom: 0px; }
        .blockquote-tip         p:last-child  { margin-bottom: 0px; }
        .blockquote-warning     p:last-child  { margin-bottom: 0px; }
        .blockquote-attention   p:last-child  { margin-bottom: 0px; }
        .blockquote-title-block { margin-bottom:   10px; }
        .blockquote-title       { font-weight:     bold; }
        
        table.api-item            { border-collapse: separate; background-color: Black; color: White; width: 100%; margin-top: 70px; margin-right: 0px; font-size: 110%; }
        .api-item td:first-child  { width: 33%; white-space: nowrap; }
        .api-item-title-src-ref   { text-align:  right; }
        .api-item-title-src-ref a { color:       inherit; }
        .api-item-title-kind      { font-weight: normal; font-size: 80%; }
        .api-item-title-light     { font-weight: normal; }
        .api-item-title-strong    { font-weight: bold; font-size: 100%; }
        .api-additional-infos     { font-size: 90%; white-space: break-spaces; overflow-wrap: break-word; }
        
        table.table-enumeration           { border: 1px solid LightGrey; border-collapse: collapse; width: 100%; font-size: 90%; }
        .table-enumeration td             { padding: 6px; border: 1px solid LightGrey; border-collapse: collapse; min-width: 100px; }
        .table-enumeration td:first-child { background-color: #f8f8f8; white-space: nowrap; }
        .table-enumeration a              { text-decoration: none; }
        
        .container td:last-child { width: 100%; }
        .tdname .inline-code     { background-color: revert; padding: 2px; border: revert; }
        .inline-code             { font-size: 110%; font-family: monospace; display: inline-block; background-color: #eeeeee; padding: 2px; border-radius: 5px; border: 1px dotted #cccccc; }
        .code-type               { background-color: #eeeeee; }
        .code-type a             { color: inherit; }
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
<h4>composite</h4>
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
<h4>dialogs</h4>
<ul>
<li><a href="#Gui_Dialog">Dialog</a></li>
<li><a href="#Gui_EditDlg">EditDlg</a></li>
<li><a href="#Gui_FileDlg">FileDlg</a></li>
<li><a href="#Gui_FileDlgOptions">FileDlgOptions</a></li>
<li><a href="#Gui_FileDlgState">FileDlgState</a></li>
<li><a href="#Gui_MessageDlg">MessageDlg</a></li>
</ul>
<h4>paint</h4>
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
<h4>property</h4>
<ul>
<li><a href="#Gui_PropertyList">PropertyList</a></li>
<li><a href="#Gui_PropertyListCtrl">PropertyListCtrl</a></li>
<li><a href="#Gui_PropertyListItem">PropertyListItem</a></li>
<li><a href="#Gui_PropertyListView">PropertyListView</a></li>
</ul>
<h4>richedit</h4>
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
<h4>widgets</h4>
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
<h4>wnd</h4>
<ul>
<li><a href="#Gui_FrameWnd">FrameWnd</a></li>
<li><a href="#Gui_KeyShortcut">KeyShortcut</a></li>
<li><a href="#Gui_ScrollWnd">ScrollWnd</a></li>
<li><a href="#Gui_SurfaceWnd">SurfaceWnd</a></li>
<li><a href="#Gui_Wnd">Wnd</a></li>
</ul>
<h3>Interfaces</h3>
<h4></h4>
<ul>
<li><a href="#Gui_IActionUI">IActionUI</a></li>
</ul>
<h4>richedit</h4>
<ul>
<li><a href="#Gui_IRichEditLexer">IRichEditLexer</a></li>
</ul>
<h4>wnd</h4>
<ul>
<li><a href="#Gui_IWnd">IWnd</a></li>
</ul>
<h3>Enums</h3>
<h4></h4>
<ul>
<li><a href="#Gui_EventKind">EventKind</a></li>
<li><a href="#Gui_MovieSource">MovieSource</a></li>
<li><a href="#Gui_SurfaceFlags">SurfaceFlags</a></li>
</ul>
<h4>composite</h4>
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
<h4>dialogs</h4>
<ul>
<li><a href="#Gui_FileDlgMode">FileDlgMode</a></li>
</ul>
<h4>paint</h4>
<ul>
<li><a href="#Gui_CursorShape">CursorShape</a></li>
<li><a href="#Gui_ThemeIcons24">ThemeIcons24</a></li>
<li><a href="#Gui_ThemeIcons64">ThemeIcons64</a></li>
</ul>
<h4>richedit</h4>
<ul>
<li><a href="#Gui_RichEditCommand">RichEditCommand</a></li>
<li><a href="#Gui_RichEditFlags">RichEditFlags</a></li>
<li><a href="#Gui_RichEditForm">RichEditForm</a></li>
<li><a href="#Gui_RichEditUndoType">RichEditUndoType</a></li>
</ul>
<h4>widgets</h4>
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
<h4>wnd</h4>
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
<h3>Type Aliases</h3>
<h4></h4>
<ul>
<li><a href="#Gui_Clipboard_Format">Format</a></li>
<li><a href="#Gui_FrameCallback">FrameCallback</a></li>
</ul>
<h4>richedit</h4>
<ul>
<li><a href="#Gui_RichEditStyleRef">RichEditStyleRef</a></li>
</ul>
<h4>wnd</h4>
<ul>
<li><a href="#Gui_HookEvent">HookEvent</a></li>
<li><a href="#Gui_WndId">WndId</a></li>
</ul>
<h3>Attributes</h3>
<h4>property</h4>
<ul>
<li><a href="#Gui_PropertyList_Category">Category</a></li>
<li><a href="#Gui_PropertyList_Description">Description</a></li>
<li><a href="#Gui_PropertyList_EditSlider">EditSlider</a></li>
<li><a href="#Gui_PropertyList_Name">Name</a></li>
<li><a href="#Gui_PropertyList_SubCategory">SubCategory</a></li>
</ul>
<h3>Functions</h3>
<h4></h4>
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
<h4>composite</h4>
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
<h4>dialogs</h4>
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
<h4>paint</h4>
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
<h4>property</h4>
<ul>
<li><a href="#Gui_PropertyList_addItem">PropertyList.addItem</a></li>
<li><a href="#Gui_PropertyList_create">PropertyList.create</a></li>
<li><a href="#Gui_PropertyList_refresh">PropertyList.refresh</a></li>
<li><a href="#Gui_PropertyList_setStringFilter">PropertyList.setStringFilter</a></li>
<li><a href="#Gui_PropertyListCtrl_addItem">PropertyListCtrl.addItem</a></li>
<li><a href="#Gui_PropertyListCtrl_create">PropertyListCtrl.create</a></li>
</ul>
<h4>richedit</h4>
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
<h4>widgets</h4>
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
<h4>wnd</h4>
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
<div class="right-page">
<div class="blockquote-warning">
<b>Work in progress</b>. Generated documentation (swag doc 0.24.0)</div>
<h1>Module gui</h1>
<h1>Content</h1>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Clipboard_Format"><span class="api-item-title-kind">type alias</span> <span class="api-item-title-light">Clipboard.</span><span class="api-item-title-strong">Type Aliases</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\clipboard.win32.swg#L6" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td id="Gui_Clipboard_Format" class="code-type"><span class="SCst">Format</span></td>
<td class="code-type"><span class="STpe">u64</span></td>
<td></td>
</tr>
<tr>
<td id="Gui_FrameCallback" class="code-type"><span class="SCst">FrameCallback</span></td>
<td class="code-type"><span class="SCde"><span class="SKwd">closure</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Application">Application</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td id="Gui_HookEvent" class="code-type"><span class="SCst">HookEvent</span></td>
<td class="code-type"><span class="SCde"><span class="SKwd">closure</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>, *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Event">Event</a></span>)-&gt;<span class="STpe">bool</span></span></td>
<td></td>
</tr>
<tr>
<td id="Gui_RichEditStyleRef" class="code-type"><span class="SCst">RichEditStyleRef</span></td>
<td class="code-type"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td id="Gui_WndId" class="code-type"><span class="SCst">WndId</span></td>
<td class="code-type"><span class="STpe">string</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ActionContext"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">ActionContext</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\action.swg#L1" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde">wnd</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">id</span></td>
<td class="code-type"><span class="STpe">string</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_AnchorStyle"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">AnchorStyle</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L18" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">None</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">X</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Y</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Width</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Height</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Application"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">Application</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L6" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> native</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ApplicationNative">ApplicationNative</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sigFrame</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SigArray">SigArray</a></span>'(<span class="SKwd">closure</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Application">Application</a></span>))</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">appIcon</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Image</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">renderer</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">RenderOgl</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">theme</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Theme">Theme</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">style</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeStyle">ThemeStyle</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">keyb</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Input</span>.<span class="SCst">Keyboard</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">mouse</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Input</span>.<span class="SCst">Mouse</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">mainSurface</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Surface">Surface</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">postedEvents</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Event">Event</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">surfaces</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Surface">Surface</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">postDestroy</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">hookEvents</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">cursorStack</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'(<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Cursor">Cursor</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">endModalExit</span></td>
<td class="code-type"><span class="STpe">string</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">inModalLoop</span></td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">quitCode</span></td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">timing</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Time</span>.<span class="SCst">FrameTiming</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">mustQuit</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">endModal</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">isActivated</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">fadeDisabledSurface</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">mutexPostedEvents</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Sync</span>.<span class="SCst">Mutex</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">duringTimerEvents</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">timers</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Timer">Timer</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">timersToAdd</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Timer">Timer</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">timersToDelete</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Timer">Timer</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">frameEvents</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">modalSurfaces</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Surface">Surface</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">toDelete</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'({ptr: *<span class="STpe">void</span>, type: <span class="SKwd">const</span> *<span class="SCst">Swag</span>.<span class="SCst">TypeInfo</span>})</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">mouseEnterWnd</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">mouseCaptureWnd</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">keybFocusWnd</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">maxRunFrame</span></td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">configPath</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">String</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">hotKeys</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'(<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_KeyShortcut">KeyShortcut</a></span>)</span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_Application_addTimer"><span class="SCde"><span class="SFct">addTimer</span>()</spa</a></td>
<td>Register a new timer for the given <span class="inline-code">target</span>. </td>
</tr>
<tr>
<td><a href="#Gui_Application_createSurface"><span class="SCde"><span class="SFct">createSurface</span>()</spa</a></td>
<td>Creates a new surface. </td>
</tr>
<tr>
<td><a href="#Gui_Application_deleteTimer"><span class="SCde"><span class="SFct">deleteTimer</span>()</spa</a></td>
<td>Delete and unregister timer. </td>
</tr>
<tr>
<td><a href="#Gui_Application_destroyWnd"><span class="SCde"><span class="SFct">destroyWnd</span>()</spa</a></td>
<td>Destroy a window  To cleanly close a window, you should call wnd.destroy(). </td>
</tr>
<tr>
<td><a href="#Gui_Application_doModalLoop"><span class="SCde"><span class="SFct">doModalLoop</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Application_exitModal"><span class="SCde"><span class="SFct">exitModal</span>()</spa</a></td>
<td>End current modal loop. </td>
</tr>
<tr>
<td><a href="#Gui_Application_getDirectoryIcon"><span class="SCde"><span class="SFct">getDirectoryIcon</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Application_getDt"><span class="SCde"><span class="SFct">getDt</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Application_getFileIcon"><span class="SCde"><span class="SFct">getFileIcon</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Application_getHotKeyShortcut"><span class="SCde"><span class="SFct">getHotKeyShortcut</span>()</spa</a></td>
<td>Get the id associated with a shortcut. null if none. </td>
</tr>
<tr>
<td><a href="#Gui_Application_getHotKeyShortcutNameFor"><span class="SCde"><span class="SFct">getHotKeyShortcutNameFor</span>()</spa</a></td>
<td>Get the name associated with a given id shortcut. </td>
</tr>
<tr>
<td><a href="#Gui_Application_getKeyboard"><span class="SCde"><span class="SFct">getKeyboard</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Application_getMouse"><span class="SCde"><span class="SFct">getMouse</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Application_getMouseCapture"><span class="SCde"><span class="SFct">getMouseCapture</span>()</spa</a></td>
<td>Return the captured wnd for mouse. </td>
</tr>
<tr>
<td><a href="#Gui_Application_getRenderer"><span class="SCde"><span class="SFct">getRenderer</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Application_getWndAtMouse"><span class="SCde"><span class="SFct">getWndAtMouse</span>()</spa</a></td>
<td>Returns the window under the given point. </td>
</tr>
<tr>
<td><a href="#Gui_Application_invalidate"><span class="SCde"><span class="SFct">invalidate</span>()</spa</a></td>
<td>Force all surfaces to be painted. </td>
</tr>
<tr>
<td><a href="#Gui_Application_loadState"><span class="SCde"><span class="SFct">loadState</span>()</spa</a></td>
<td>Load the application state. </td>
</tr>
<tr>
<td><a href="#Gui_Application_popCursor"><span class="SCde"><span class="SFct">popCursor</span>()</spa</a></td>
<td>Pop a new mouse cursor. </td>
</tr>
<tr>
<td><a href="#Gui_Application_postEvent"><span class="SCde"><span class="SFct">postEvent</span>()</spa</a></td>
<td>Post a new event (thread safe). </td>
</tr>
<tr>
<td><a href="#Gui_Application_postQuitEvent"><span class="SCde"><span class="SFct">postQuitEvent</span>()</spa</a></td>
<td>Ask to exit the application. </td>
</tr>
<tr>
<td><a href="#Gui_Application_pushCursor"><span class="SCde"><span class="SFct">pushCursor</span>()</spa</a></td>
<td>Push a new mouse cursor. </td>
</tr>
<tr>
<td><a href="#Gui_Application_registerFrameEvent"><span class="SCde"><span class="SFct">registerFrameEvent</span>()</spa</a></td>
<td>Register a window to receive a FrameEvent each running loop. </td>
</tr>
<tr>
<td><a href="#Gui_Application_registerHookEvents"><span class="SCde"><span class="SFct">registerHookEvents</span>()</spa</a></td>
<td>Register a window to receive hook events. </td>
</tr>
<tr>
<td><a href="#Gui_Application_registerHotKey"><span class="SCde"><span class="SFct">registerHotKey</span>()</spa</a></td>
<td>Register a global os key action. </td>
</tr>
<tr>
<td><a href="#Gui_Application_run"><span class="SCde"><span class="SFct">run</span>()</spa</a></td>
<td>Run until exit. </td>
</tr>
<tr>
<td><a href="#Gui_Application_runFrame"><span class="SCde"><span class="SFct">runFrame</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Application_runSurface"><span class="SCde"><span class="SFct">runSurface</span>()</spa</a></td>
<td>Create a main surface, and run. </td>
</tr>
<tr>
<td><a href="#Gui_Application_saveState"><span class="SCde"><span class="SFct">saveState</span>()</spa</a></td>
<td>Save the application state. </td>
</tr>
<tr>
<td><a href="#Gui_Application_sendEvent"><span class="SCde"><span class="SFct">sendEvent</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Application_setActivated"><span class="SCde"><span class="SFct">setActivated</span>()</spa</a></td>
<td>Set activated state. </td>
</tr>
<tr>
<td><a href="#Gui_Application_setAppIcon"><span class="SCde"><span class="SFct">setAppIcon</span>()</spa</a></td>
<td>Associate a configuration file. </td>
</tr>
<tr>
<td><a href="#Gui_Application_setConfigPath"><span class="SCde"><span class="SFct">setConfigPath</span>()</spa</a></td>
<td>Associate a configuration file. </td>
</tr>
<tr>
<td><a href="#Gui_Application_unregisterFrameEvent"><span class="SCde"><span class="SFct">unregisterFrameEvent</span>()</spa</a></td>
<td>Unregister a window that receives a FrameEvent each running loop. </td>
</tr>
<tr>
<td><a href="#Gui_Application_unregisterHookEvents"><span class="SCde"><span class="SFct">unregisterHookEvents</span>()</spa</a></td>
<td>Unregister a window to receive hook events. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Application_addTimer"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Application.</span><span class="api-item-title-strong">addTimer</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L711" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Register a new timer for the given <span class="inline-code">target</span>. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">addTimer</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, time: <span class="SCst">Core</span>.<span class="SCst">Time</span>.<span class="SCst">Duration</span>, target: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Timer">Timer</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Application_createSurface"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Application.</span><span class="api-item-title-strong">createSurface</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L542" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Creates a new surface. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">createSurface</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, x: <span class="STpe">s32</span>, y: <span class="STpe">s32</span>, width: <span class="STpe">s32</span>, height: <span class="STpe">s32</span>, flags = <span class="SCst"><a href="#Gui_SurfaceFlags">SurfaceFlags</a></span>.<span class="SCst">OverlappedWindow</span>, view: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span> = <span class="SKwd">null</span>, hook: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_HookEvent">HookEvent</a></span> = <span class="SKwd">null</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Surface">Surface</a></span> <span class="SKwd">throw</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Application_deleteTimer"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Application.</span><span class="api-item-title-strong">deleteTimer</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L724" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Delete and unregister timer. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">deleteTimer</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, timer: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Timer">Timer</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Application_destroyWnd"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Application.</span><span class="api-item-title-strong">destroyWnd</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L638" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Destroy a window  To cleanly close a window, you should call wnd.destroy(). </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">destroyWnd</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, wnd: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Application_doModalLoop"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Application.</span><span class="api-item-title-strong">doModalLoop</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L451" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">doModalLoop</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, surface: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Surface">Surface</a></span>, cb: <span class="SKwd">closure</span>(<span class="SKwd">self</span>) = <span class="SKwd">null</span>)-&gt;<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Application_exitModal"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Application.</span><span class="api-item-title-strong">exitModal</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L806" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>End current modal loop. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">exitModal</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, exitId: <span class="STpe">string</span> = <span class="SStr">""</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Application_getDirectoryIcon"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Application.</span><span class="api-item-title-strong">getDirectoryIcon</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L813" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getDirectoryIcon</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, name: <span class="STpe">string</span>, small = <span class="SKwd">true</span>)-&gt;<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Icon">Icon</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Application_getDt"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Application.</span><span class="api-item-title-strong">getDt</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L523" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getDt</span>(<span class="SKwd">using</span> <span class="SKwd">const</span> <span class="SKwd">self</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Application_getFileIcon"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Application.</span><span class="api-item-title-strong">getFileIcon</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L812" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getFileIcon</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, name: <span class="STpe">string</span>, small = <span class="SKwd">true</span>)-&gt;<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Icon">Icon</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Application_getHotKeyShortcut"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Application.</span><span class="api-item-title-strong">getHotKeyShortcut</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L833" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the id associated with a shortcut. null if none. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getHotKeyShortcut</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, mdf: <span class="SCst">Core</span>.<span class="SCst">Input</span>.<span class="SCst">KeyModifiers</span>, key: <span class="SCst">Core</span>.<span class="SCst">Input</span>.<span class="SCst">Key</span>)-&gt;{id: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span>, wnd: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>}</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Application_getHotKeyShortcutNameFor"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Application.</span><span class="api-item-title-strong">getHotKeyShortcutNameFor</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L845" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the name associated with a given id shortcut. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getHotKeyShortcutNameFor</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, wndId: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span>)-&gt;<span class="SCst">Core</span>.<span class="SCst">String</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Application_getKeyboard"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Application.</span><span class="api-item-title-strong">getKeyboard</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L521" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getKeyboard</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;*<span class="SCst">Core</span>.<span class="SCst">Input</span>.<span class="SCst">Keyboard</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Application_getMouse"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Application.</span><span class="api-item-title-strong">getMouse</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L522" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getMouse</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;*<span class="SCst">Core</span>.<span class="SCst">Input</span>.<span class="SCst">Mouse</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Application_getMouseCapture"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Application.</span><span class="api-item-title-strong">getMouseCapture</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L631" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Return the captured wnd for mouse. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getMouseCapture</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Application_getRenderer"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Application.</span><span class="api-item-title-strong">getRenderer</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L520" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getRenderer</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;*<span class="SCst">Pixel</span>.<span class="SCst">RenderOgl</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Application_getWndAtMouse"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Application.</span><span class="api-item-title-strong">getWndAtMouse</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L661" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the window under the given point. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getWndAtMouse</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Application_invalidate"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Application.</span><span class="api-item-title-strong">invalidate</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L684" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Force all surfaces to be painted. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">invalidate</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Application_loadState"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Application.</span><span class="api-item-title-strong">loadState</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L616" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Load the application state. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">loadState</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>) <span class="SKwd">throw</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Application_popCursor"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Application.</span><span class="api-item-title-strong">popCursor</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L773" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Pop a new mouse cursor. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">popCursor</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Application_postEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Application.</span><span class="api-item-title-strong">postEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L699" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Post a new event (thread safe). </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">postEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, event: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Event">Event</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Application_postQuitEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Application.</span><span class="api-item-title-strong">postQuitEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L691" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Ask to exit the application. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">postQuitEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, quitCode: <span class="STpe">s32</span> = <span class="SNum">0</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Application_pushCursor"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Application.</span><span class="api-item-title-strong">pushCursor</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L767" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Push a new mouse cursor. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">pushCursor</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, cursor: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Cursor">Cursor</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Application_registerFrameEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Application.</span><span class="api-item-title-strong">registerFrameEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L741" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Register a window to receive a FrameEvent each running loop. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">registerFrameEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, target: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Application_registerHookEvents"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Application.</span><span class="api-item-title-strong">registerHookEvents</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L754" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Register a window to receive hook events. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">registerHookEvents</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, target: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Application_registerHotKey"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Application.</span><span class="api-item-title-strong">registerHotKey</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L816" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Register a global os key action. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">registerHotKey</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, mdf: <span class="SCst">Core</span>.<span class="SCst">Input</span>.<span class="SCst">KeyModifiers</span>, key: <span class="SCst">Core</span>.<span class="SCst">Input</span>.<span class="SCst">Key</span>, wndId: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span>, target: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>) <span class="SKwd">throw</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Application_run"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Application.</span><span class="api-item-title-strong">run</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L790" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Run until exit. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">run</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="STpe">s32</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Application_runFrame"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Application.</span><span class="api-item-title-strong">runFrame</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L425" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">runFrame</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Application_runSurface"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Application.</span><span class="api-item-title-strong">runSurface</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L527" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Create a main surface, and run. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">runSurface</span>(x: <span class="STpe">s32</span>, y: <span class="STpe">s32</span>, width: <span class="STpe">s32</span>, height: <span class="STpe">s32</span>, title: <span class="STpe">string</span> = <span class="SKwd">null</span>, view: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span> = <span class="SKwd">null</span>, borders = <span class="SKwd">true</span>, canSize = <span class="SKwd">true</span>, hook: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_HookEvent">HookEvent</a></span> = <span class="SKwd">null</span>, init: <span class="SKwd">func</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Application">Application</a></span>) = <span class="SKwd">null</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Application_saveState"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Application.</span><span class="api-item-title-strong">saveState</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L600" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Save the application state. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">saveState</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>) <span class="SKwd">throw</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Application_sendEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Application.</span><span class="api-item-title-strong">sendEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L705" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">sendEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, event: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Event">Event</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Application_setActivated"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Application.</span><span class="api-item-title-strong">setActivated</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L673" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set activated state. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setActivated</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, activated: <span class="STpe">bool</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Application_setAppIcon"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Application.</span><span class="api-item-title-strong">setAppIcon</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L586" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Associate a configuration file. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setAppIcon</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, img: <span class="SCst">Pixel</span>.<span class="SCst">Image</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Application_setConfigPath"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Application.</span><span class="api-item-title-strong">setConfigPath</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L580" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Associate a configuration file. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setConfigPath</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, path: <span class="STpe">string</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Application_unregisterFrameEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Application.</span><span class="api-item-title-strong">unregisterFrameEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L748" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Unregister a window that receives a FrameEvent each running loop. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">unregisterFrameEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, target: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Application_unregisterHookEvents"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Application.</span><span class="api-item-title-strong">unregisterHookEvents</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.swg#L761" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Unregister a window to receive hook events. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">unregisterHookEvents</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, target: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ApplicationNative"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">ApplicationNative</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\application.win32.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde">fileSmallIconsImage</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Image</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">fileSmallIconsImageList</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ImageList">ImageList</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">mapFileSmallIcons</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">HashTable</span>'(<span class="STpe">string</span>, <span class="STpe">s32</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">fileBigIconsImage</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Image</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">fileBigIconsImageList</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ImageList">ImageList</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">mapFileBigIcons</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">HashTable</span>'(<span class="STpe">string</span>, <span class="STpe">s32</span>)</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_BackgroundStyle"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">BackgroundStyle</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L27" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">Transparent</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Window</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Dialog</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">DialogBar</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">View</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_BlendColor"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">BlendColor</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\blendcolor.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde">curVec4</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Vector4</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">startVec4</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Vector4</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">destVec4</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Vector4</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">factor</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">start</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">speed</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_BlendColor_apply"><span class="SCde"><span class="SFct">apply</span>()</spa</a></td>
<td>Apply the color lerp to reach <span class="inline-code">target</span>. </td>
</tr>
<tr>
<td><a href="#Gui_BlendColor_cur"><span class="SCde"><span class="SFct">cur</span>()</spa</a></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_BlendColor_apply"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">BlendColor.</span><span class="api-item-title-strong">apply</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\blendcolor.swg#L17" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Apply the color lerp to reach <span class="inline-code">target</span>. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">apply</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, wnd: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>, target: <span class="SCst">Pixel</span>.<span class="SCst">Color</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_BlendColor_cur"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">BlendColor.</span><span class="api-item-title-strong">cur</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\blendcolor.swg#L60" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">cur</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Button"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">Button</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\button.swg#L1" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> wnd</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">icon</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Icon">Icon</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sigPressed</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SigArray">SigArray</a></span>'(<span class="SKwd">closure</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Button">Button</a></span>))</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sigRightPressed</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SigArray">SigArray</a></span>'(<span class="SKwd">closure</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Button">Button</a></span>))</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">isPressed</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">isPressing</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">isHot</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">isIn</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ButtonCheckState"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">ButtonCheckState</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\checkbutton.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">UnChecked</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Checked</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Undefined</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_CheckButton"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">CheckButton</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\checkbutton.swg#L19" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> button</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Button">Button</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">checked</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ButtonCheckState">ButtonCheckState</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">checkButtonFlags</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_CheckButtonFlags">CheckButtonFlags</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sigChanged</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SigArray">SigArray</a></span>'(<span class="SKwd">closure</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_CheckButton">CheckButton</a></span>))</span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_CheckButton_create"><span class="SCde"><span class="SFct">create</span>()</spa</a></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_CheckButton_IWnd_onMouseEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onMouseEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\checkbutton.swg#L120" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onMouseEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_MouseEvent">MouseEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_CheckButton_IWnd_onPaintEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onPaintEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\checkbutton.swg#L30" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onPaintEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_PaintEvent">PaintEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_CheckButton_create"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">CheckButton.</span><span class="api-item-title-strong">create</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\checkbutton.swg#L139" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">create</span>(parent: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>, name: <span class="STpe">string</span>, position: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span> = {}, id: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span> = <span class="SKwd">null</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_CheckButton">CheckButton</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_CheckButtonFlags"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">CheckButtonFlags</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\checkbutton.swg#L12" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">Zero</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">ThreeState</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">RightAlign</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Clipboard_addData"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Clipboard.</span><span class="api-item-title-strong">addData</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\clipboard.win32.swg#L27" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add data of a given format to the clipboard. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">addData</span>(fmt: <span class="SCst">Gui</span>.<span class="SCst">Clipboard</span>.<span class="SCst"><a href="#Gui_Clipboard_Format">Format</a></span>, data: <span class="SKwd">const</span> [..] <span class="STpe">u8</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Clipboard_addImage"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Clipboard.</span><span class="api-item-title-strong">addImage</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\clipboard.win32.swg#L85" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add an image. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">addImage</span>(image: <span class="SCst">Pixel</span>.<span class="SCst">Image</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Clipboard_addString"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Clipboard.</span><span class="api-item-title-strong">addString</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\clipboard.win32.swg#L200" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add string to clipboard. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">addString</span>(str: <span class="STpe">string</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Clipboard_getData"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Clipboard.</span><span class="api-item-title-strong">getData</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\clipboard.win32.swg#L44" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get data of the given format. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getData</span>(fmt: <span class="SCst">Gui</span>.<span class="SCst">Clipboard</span>.<span class="SCst"><a href="#Gui_Clipboard_Format">Format</a></span>)-&gt;<span class="SCst">Core</span>.<span class="SCst">Array</span>'(<span class="STpe">u8</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Clipboard_getImage"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Clipboard.</span><span class="api-item-title-strong">getImage</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\clipboard.win32.swg#L142" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get image from clipboard. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getImage</span>()-&gt;<span class="SCst">Pixel</span>.<span class="SCst">Image</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Clipboard_getString"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Clipboard.</span><span class="api-item-title-strong">getString</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\clipboard.win32.swg#L229" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get utf8 string from clipboard. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getString</span>()-&gt;<span class="SCst">Core</span>.<span class="SCst">String</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Clipboard_hasFormat"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Clipboard.</span><span class="api-item-title-strong">hasFormat</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\clipboard.win32.swg#L17" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns true if the clipboard contains the given format. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">hasFormat</span>(fmt: <span class="SCst">Gui</span>.<span class="SCst">Clipboard</span>.<span class="SCst"><a href="#Gui_Clipboard_Format">Format</a></span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Clipboard_hasImage"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Clipboard.</span><span class="api-item-title-strong">hasImage</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\clipboard.win32.swg#L70" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns true if the clipbboard contains an image. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">hasImage</span>()-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Clipboard_hasString"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Clipboard.</span><span class="api-item-title-strong">hasString</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\clipboard.win32.swg#L185" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns true if the clipboard contains a string. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">hasString</span>()-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Clipboard_registerFormat"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Clipboard.</span><span class="api-item-title-strong">registerFormat</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\clipboard.win32.swg#L11" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Register a new format. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">registerFormat</span>(name: <span class="STpe">string</span>)-&gt;<span class="SCst">Gui</span>.<span class="SCst">Clipboard</span>.<span class="SCst"><a href="#Gui_Clipboard_Format">Format</a></span> <span class="SKwd">throw</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ColorPicker"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">ColorPicker</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\colorpicker.swg#L28" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> wnd</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">hue</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sat</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">lum</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">a</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">r</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">g</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">b</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">mode</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ColorPickerMode">ColorPickerMode</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sigChanged</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SigArray">SigArray</a></span>'(<span class="SKwd">closure</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ColorPicker">ColorPicker</a></span>))</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">dirty</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">isMoving</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">img</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Image</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">texture</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Texture</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">xCur</span></td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">yCur</span></td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_ColorPicker_change"><span class="SCde"><span class="SFct">change</span>()</spa</a></td>
<td>Change one component, HSL or alpha. </td>
</tr>
<tr>
<td><a href="#Gui_ColorPicker_create"><span class="SCde"><span class="SFct">create</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_ColorPicker_getColor"><span class="SCde"><span class="SFct">getColor</span>()</spa</a></td>
<td>Returns the selected color. </td>
</tr>
<tr>
<td><a href="#Gui_ColorPicker_setColor"><span class="SCde"><span class="SFct">setColor</span>()</spa</a></td>
<td>Set the base color, and repaint. </td>
</tr>
<tr>
<td><a href="#Gui_ColorPicker_setMode"><span class="SCde"><span class="SFct">setMode</span>()</spa</a></td>
<td>Set the color picker box mode. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ColorPicker_IWnd_computeXY"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">computeXY</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\colorpicker.swg#L45" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">computeXY</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ColorPicker_IWnd_onMouseEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onMouseEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\colorpicker.swg#L277" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onMouseEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_MouseEvent">MouseEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ColorPicker_IWnd_onPaintEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onPaintEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\colorpicker.swg#L118" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onPaintEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_PaintEvent">PaintEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ColorPicker_IWnd_onPostPaintEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onPostPaintEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\colorpicker.swg#L247" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onPostPaintEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_PaintEvent">PaintEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ColorPicker_change"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ColorPicker.</span><span class="api-item-title-strong">change</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\colorpicker.swg#L360" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Change one component, HSL or alpha. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">change</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, h: <span class="STpe">f32</span> = -<span class="SNum">1</span>, s: <span class="STpe">f32</span> = -<span class="SNum">1</span>, l: <span class="STpe">f32</span> = -<span class="SNum">1</span>, a: <span class="STpe">f32</span> = -<span class="SNum">1</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ColorPicker_create"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ColorPicker.</span><span class="api-item-title-strong">create</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\colorpicker.swg#L349" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">create</span>(parent: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>, position: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span> = {}, id: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span> = <span class="SKwd">null</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ColorPicker">ColorPicker</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ColorPicker_getColor"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ColorPicker.</span><span class="api-item-title-strong">getColor</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\colorpicker.swg#L406" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the selected color. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getColor</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ColorPicker_setColor"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ColorPicker.</span><span class="api-item-title-strong">setColor</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\colorpicker.swg#L396" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the base color, and repaint. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setColor</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, col: <span class="SCst">Pixel</span>.<span class="SCst">Color</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ColorPicker_setMode"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ColorPicker.</span><span class="api-item-title-strong">setMode</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\colorpicker.swg#L412" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the color picker box mode. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setMode</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, mode: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ColorPickerMode">ColorPickerMode</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ColorPickerCtrl"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">ColorPickerCtrl</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\colorpickerctrl.swg#L14" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> frameWnd</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_FrameWnd">FrameWnd</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sigChanged</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SigArray">SigArray</a></span>'(<span class="SKwd">closure</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ColorPickerCtrl">ColorPickerCtrl</a></span>))</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">oldColor</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">color</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">oldColorDone</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">mode</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ColorPickerViewMode">ColorPickerViewMode</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">staticRes</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">pickBox</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ColorPicker">ColorPicker</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">pickBar</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ColorPicker">ColorPicker</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">pickAlpha</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ColorPicker">ColorPicker</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">editR</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_EditBox">EditBox</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">editG</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_EditBox">EditBox</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">editB</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_EditBox">EditBox</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">editA</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_EditBox">EditBox</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">editH</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_EditBox">EditBox</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">radioH</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RadioButton">RadioButton</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">radioL</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RadioButton">RadioButton</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">radioS</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RadioButton">RadioButton</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">radioR</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RadioButton">RadioButton</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">radioG</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RadioButton">RadioButton</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">radioB</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RadioButton">RadioButton</a></span></span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_ColorPickerCtrl_create"><span class="SCde"><span class="SFct">create</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_ColorPickerCtrl_getColor"><span class="SCde"><span class="SFct">getColor</span>()</spa</a></td>
<td>Returns the selected color. </td>
</tr>
<tr>
<td><a href="#Gui_ColorPickerCtrl_setColor"><span class="SCde"><span class="SFct">setColor</span>()</spa</a></td>
<td>Set the selected color. </td>
</tr>
<tr>
<td><a href="#Gui_ColorPickerCtrl_setMode"><span class="SCde"><span class="SFct">setMode</span>()</spa</a></td>
<td>Change the display mode. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ColorPickerCtrl_IWnd_onPaintEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onPaintEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\colorpickerctrl.swg#L41" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onPaintEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_PaintEvent">PaintEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ColorPickerCtrl_create"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ColorPickerCtrl.</span><span class="api-item-title-strong">create</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\colorpickerctrl.swg#L116" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">create</span>(parent: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>, position: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span>, id: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span> = <span class="SKwd">null</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ColorPickerCtrl">ColorPickerCtrl</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ColorPickerCtrl_getColor"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ColorPickerCtrl.</span><span class="api-item-title-strong">getColor</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\colorpickerctrl.swg#L354" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the selected color. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getColor</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ColorPickerCtrl_setColor"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ColorPickerCtrl.</span><span class="api-item-title-strong">setColor</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\colorpickerctrl.swg#L360" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the selected color. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setColor</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, color: <span class="SCst">Pixel</span>.<span class="SCst">Color</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ColorPickerCtrl_setMode"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ColorPickerCtrl.</span><span class="api-item-title-strong">setMode</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\colorpickerctrl.swg#L326" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Change the display mode. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setMode</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, mode: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ColorPickerViewMode">ColorPickerViewMode</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ColorPickerMode"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">ColorPickerMode</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\colorpicker.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">SaturationLightness</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">HueSaturation</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">HueLightness</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">GreenBlue</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">RedBlue</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">RedGreen</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">HueVert</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">HueHorz</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">LightnessVert</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">LightnessHorz</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">SaturationVert</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">SaturationHorz</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">AlphaHorz</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">AlphaVert</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">RedHorz</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">RedVert</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">GreenHorz</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">GreenVert</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">BlueHorz</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">BlueVert</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ColorPickerViewMode"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">ColorPickerViewMode</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\colorpickerctrl.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">SaturationLightness</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">HueSaturation</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">HueLightness</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">GreenBlue</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">RedBlue</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">RedGreen</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ComboBox"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">ComboBox</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\combobox.swg#L17" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> wnd</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">kind</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ComboBoxKind">ComboBoxKind</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">minWidthPopup</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">maxHeightPopup</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sigChanged</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SigArray">SigArray</a></span>'(<span class="SKwd">closure</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ComboBox">ComboBox</a></span>, <span class="STpe">u32</span>))</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">editBox</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_EditBox">EditBox</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">isHot</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">popup</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_PopupListCtrl">PopupListCtrl</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">selectedIdx</span></td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">maxIconSize</span></td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">aniBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_BlendColor">BlendColor</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">aniBorder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_BlendColor">BlendColor</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">aniText</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_BlendColor">BlendColor</a></span></span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_ComboBox_addItem"><span class="SCde"><span class="SFct">addItem</span>()</spa</a></td>
<td>Add a new item. </td>
</tr>
<tr>
<td><a href="#Gui_ComboBox_addSeparator"><span class="SCde"><span class="SFct">addSeparator</span>()</spa</a></td>
<td>Add a separator. </td>
</tr>
<tr>
<td><a href="#Gui_ComboBox_clear"><span class="SCde"><span class="SFct">clear</span>()</spa</a></td>
<td>Remove all items. </td>
</tr>
<tr>
<td><a href="#Gui_ComboBox_create"><span class="SCde"><span class="SFct">create</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_ComboBox_getSelectedItem"><span class="SCde"><span class="SFct">getSelectedItem</span>()</spa</a></td>
<td>Returns the selected item. </td>
</tr>
<tr>
<td><a href="#Gui_ComboBox_selectItem"><span class="SCde"><span class="SFct">selectItem</span>()</spa</a></td>
<td>Set the selected item index. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ComboBox_IWnd_onMouseEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onMouseEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\combobox.swg#L125" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onMouseEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_MouseEvent">MouseEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ComboBox_IWnd_onPaintEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onPaintEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\combobox.swg#L52" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onPaintEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_PaintEvent">PaintEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ComboBox_IWnd_onResizeEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onResizeEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\combobox.swg#L40" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onResizeEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ResizeEvent">ResizeEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ComboBox_addItem"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ComboBox.</span><span class="api-item-title-strong">addItem</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\combobox.swg#L223" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a new item. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">addItem</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, name: <span class="STpe">string</span>, id: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span> = <span class="SKwd">null</span>, icon: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Icon">Icon</a></span> = {}, userData0: *<span class="STpe">void</span> = <span class="SKwd">null</span>, userData1: *<span class="STpe">void</span> = <span class="SKwd">null</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ComboBoxItem">ComboBoxItem</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ComboBox_addSeparator"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ComboBox.</span><span class="api-item-title-strong">addSeparator</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\combobox.swg#L242" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a separator. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">addSeparator</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ComboBox_clear"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ComboBox.</span><span class="api-item-title-strong">clear</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\combobox.swg#L214" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Remove all items. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">clear</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ComboBox_create"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ComboBox.</span><span class="api-item-title-strong">create</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\combobox.swg#L192" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">create</span>(parent: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>, position: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span> = {}, id: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span> = <span class="SKwd">null</span>, kind = <span class="SCst"><a href="#Gui_ComboBoxKind">ComboBoxKind</a></span>.<span class="SCst">Select</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ComboBox">ComboBox</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ComboBox_getSelectedItem"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ComboBox.</span><span class="api-item-title-strong">getSelectedItem</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\combobox.swg#L250" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the selected item. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getSelectedItem</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ComboBoxItem">ComboBoxItem</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ComboBox_selectItem"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ComboBox.</span><span class="api-item-title-strong">selectItem</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\combobox.swg#L258" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the selected item index. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">selectItem</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, idx: <span class="STpe">u32</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ComboBoxItem"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">ComboBoxItem</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\combobox.swg#L11" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> base</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Item">Item</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">isSeparator</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ComboBoxKind"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">ComboBoxKind</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\combobox.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">Select</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Edit</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">CheckBox</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ComboCtrl"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">ComboCtrl</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\comboctrl.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> frameWnd</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_FrameWnd">FrameWnd</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">labelSize</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">comboSize</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">label</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Label">Label</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">combo</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ComboBox">ComboBox</a></span></span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_ComboCtrl_create"><span class="SCde"><span class="SFct">create</span>()</spa</a></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ComboCtrl_IWnd_onResizeEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onResizeEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\comboctrl.swg#L16" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onResizeEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ResizeEvent">ResizeEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ComboCtrl_create"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ComboCtrl.</span><span class="api-item-title-strong">create</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\comboctrl.swg#L31" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">create</span>(parent: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>, name: <span class="STpe">string</span>, position: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span> = {}, id: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span> = <span class="SKwd">null</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ComboCtrl">ComboCtrl</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_CommandEvent"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">CommandEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\events.swg#L69" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> base</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Event">Event</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">id</span></td>
<td class="code-type"><span class="STpe">string</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">source</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_CommandStateEvent"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">CommandStateEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\events.swg#L76" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> base</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Event">Event</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">id</span></td>
<td class="code-type"><span class="STpe">string</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">source</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">icon</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Icon">Icon</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">name</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">String</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">longName</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">String</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">shortcut</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">String</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">toolTip</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">String</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">setFlags</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_CommandStateEvent">CommandStateEvent</a></span>.<span class="SCst">SetFlags</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">disabled</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">checked</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">hidden</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_CreateEvent"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">CreateEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\events.swg#L119" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> base</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Event">Event</a></span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Cursor"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">Cursor</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\cursor.swg#L20" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> native</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_NativeCursor">NativeCursor</a></span></span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_Cursor_apply"><span class="SCde"><span class="SFct">apply</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Cursor_clear"><span class="SCde"><span class="SFct">clear</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Cursor_from"><span class="SCde"><span class="SFct">from</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Cursor_wait"><span class="SCde"><span class="SFct">wait</span>()</spa</a></td>
<td>Force the wait cursor. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Cursor_apply"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Cursor.</span><span class="api-item-title-strong">apply</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\cursor.win32.swg#L43" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">apply</span>(cursor: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Cursor">Cursor</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Cursor_clear"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Cursor.</span><span class="api-item-title-strong">clear</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\cursor.win32.swg#L38" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">clear</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Cursor_from"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Cursor.</span><span class="api-item-title-strong">from</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\cursor.win32.swg#L33" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">from</span>(shape: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_CursorShape">CursorShape</a></span>)-&gt;<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Cursor">Cursor</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Cursor_wait"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Cursor.</span><span class="api-item-title-strong">wait</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\cursor.swg#L28" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Force the wait cursor. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">wait</span>()</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_CursorShape"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">CursorShape</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\cursor.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">Arrow</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">SizeWE</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">SizeNS</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">SizeNWSE</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">SizeNESW</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">SizeAll</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Cross</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Help</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Hand</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">IBeam</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">No</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Wait</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_DestroyEvent"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">DestroyEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\events.swg#L129" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> base</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Event">Event</a></span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Dialog"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">Dialog</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\dialog.swg#L3" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> wnd</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">wndBottom</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_StackLayoutCtrl">StackLayoutCtrl</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">buttons</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_PushButton">PushButton</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sigPressedButton</span></td>
<td class="code-type"><span class="SCde"><span class="SKwd">closure</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Dialog">Dialog</a></span>, <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span>)-&gt;<span class="STpe">bool</span></span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_Dialog_addButton"><span class="SCde"><span class="SFct">addButton</span>()</spa</a></td>
<td>Add a button. </td>
</tr>
<tr>
<td><a href="#Gui_Dialog_createDialog"><span class="SCde"><span class="SFct">createDialog</span>()</spa</a></td>
<td>Creates a simple dialog box. </td>
</tr>
<tr>
<td><a href="#Gui_Dialog_createSurface"><span class="SCde"><span class="SFct">createSurface</span>()</spa</a></td>
<td>Creates an associated surface. </td>
</tr>
<tr>
<td><a href="#Gui_Dialog_doModal"><span class="SCde"><span class="SFct">doModal</span>()</spa</a></td>
<td>Display dialog as modal, and returns the user selected window id. </td>
</tr>
<tr>
<td><a href="#Gui_Dialog_validateId"><span class="SCde"><span class="SFct">validateId</span>()</spa</a></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Dialog_IWnd_onKeyEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onKeyEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\dialog.swg#L20" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onKeyEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_KeyEvent">KeyEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Dialog_IWnd_onSysCommandEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onSysCommandEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\dialog.swg#L13" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onSysCommandEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SysCommandEvent">SysCommandEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Dialog_addButton"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Dialog.</span><span class="api-item-title-strong">addButton</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\dialog.swg#L133" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a button. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">addButton</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, str: <span class="STpe">string</span>, id: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span>, isDefault: <span class="STpe">bool</span> = <span class="SKwd">false</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_PushButton">PushButton</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Dialog_createDialog"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Dialog.</span><span class="api-item-title-strong">createDialog</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\dialog.swg#L90" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Creates a simple dialog box. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">createDialog</span>(from: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Surface">Surface</a></span>, width = <span class="SNum">512</span>, height = <span class="SNum">300</span>, title: <span class="STpe">string</span> = <span class="SKwd">null</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Dialog">Dialog</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Dialog_createSurface"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Dialog.</span><span class="api-item-title-strong">createSurface</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\dialog.swg#L100" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Creates an associated surface. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">createSurface</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, from: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Surface">Surface</a></span>, x: <span class="STpe">s32</span>, y: <span class="STpe">s32</span>, width = <span class="SNum">512</span>, height = <span class="SNum">300</span>, flags = <span class="SCst"><a href="#Gui_SurfaceFlags">SurfaceFlags</a></span>.<span class="SCst">Zero</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Surface">Surface</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Dialog_doModal"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Dialog.</span><span class="api-item-title-strong">doModal</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\dialog.swg#L124" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Display dialog as modal, and returns the user selected window id. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">doModal</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="STpe">string</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Dialog_validateId"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Dialog.</span><span class="api-item-title-strong">validateId</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\dialog.swg#L153" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">validateId</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, btnId: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_DockStyle"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">DockStyle</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L7" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">None</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Top</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Left</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Right</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Bottom</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Center</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_DwmExtendFrameIntoClientArea"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">DwmExtendFrameIntoClientArea</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.win32.swg#L17" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">DwmExtendFrameIntoClientArea</span>(hWnd: <span class="SCst">Win32</span>.<span class="SCst">HWND</span>, pMarInset: *<span class="SCst">Gui</span>.<span class="SCst">MARGINS</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_EditBox"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">EditBox</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\editbox.swg#L37" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> wnd</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">icon</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Icon">Icon</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">text</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">String</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">form</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_EditBoxForm">EditBoxForm</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">editBoxFlags</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_EditBoxFlags">EditBoxFlags</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">inputMode</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_EditBoxInputMode">EditBoxInputMode</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">maxLength</span></td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">rightMargin</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sigChanged</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SigArray">SigArray</a></span>'(<span class="SKwd">closure</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_EditBox">EditBox</a></span>))</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sigLoseFocus</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SigArray">SigArray</a></span>'(<span class="SKwd">closure</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_EditBox">EditBox</a></span>))</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sigEnterPressed</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SigArray">SigArray</a></span>'(<span class="SKwd">closure</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_EditBox">EditBox</a></span>))</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sigEscapePressed</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SigArray">SigArray</a></span>'(<span class="SKwd">closure</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_EditBox">EditBox</a></span>))</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sigCheckContent</span></td>
<td class="code-type"><span class="SCde"><span class="SKwd">closure</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_EditBox">EditBox</a></span>)-&gt;<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_EditBoxCheckResult">EditBoxCheckResult</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> minMax</span></td>
<td class="code-type"><span class="SCde">{minMaxS64: {min: <span class="STpe">s64</span>, max: <span class="STpe">s64</span>}, minMaxU64: {min: <span class="STpe">u64</span>, max: <span class="STpe">u64</span>}, minMaxF64: {min: <span class="STpe">f64</span>, max: <span class="STpe">f64</span>}}</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">aniBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_BlendColor">BlendColor</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">aniBorder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_BlendColor">BlendColor</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">isInvalid</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">isHot</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">isDragging</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">selBeg</span></td>
<td class="code-type"><span class="STpe">u64</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">selEnd</span></td>
<td class="code-type"><span class="STpe">u64</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">timerCaret</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Timer">Timer</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">visibleCaret</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">posSelBeg</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">posSelEnd</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">scrollPosX</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">countRunes</span></td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_EditBox_create"><span class="SCde"><span class="SFct">create</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_EditBox_deleteSelection"><span class="SCde"><span class="SFct">deleteSelection</span>()</spa</a></td>
<td>Delete selected text. </td>
</tr>
<tr>
<td><a href="#Gui_EditBox_getText"><span class="SCde"><span class="SFct">getText</span>()</spa</a></td>
<td>Get the associated text. </td>
</tr>
<tr>
<td><a href="#Gui_EditBox_selectAll"><span class="SCde"><span class="SFct">selectAll</span>()</spa</a></td>
<td>Select all text. </td>
</tr>
<tr>
<td><a href="#Gui_EditBox_setForm"><span class="SCde"><span class="SFct">setForm</span>()</spa</a></td>
<td>Set the editbox form. </td>
</tr>
<tr>
<td><a href="#Gui_EditBox_setInputF64"><span class="SCde"><span class="SFct">setInputF64</span>()</spa</a></td>
<td>Editbox will edit floating points. </td>
</tr>
<tr>
<td><a href="#Gui_EditBox_setInputS64"><span class="SCde"><span class="SFct">setInputS64</span>()</spa</a></td>
<td>Editbox will edit signed integers. </td>
</tr>
<tr>
<td><a href="#Gui_EditBox_setInputU64"><span class="SCde"><span class="SFct">setInputU64</span>()</spa</a></td>
<td>Editbox will edit unsigned integers. </td>
</tr>
<tr>
<td><a href="#Gui_EditBox_setNote"><span class="SCde"><span class="SFct">setNote</span>()</spa</a></td>
<td>Set the editbox note. </td>
</tr>
<tr>
<td><a href="#Gui_EditBox_setSelection"><span class="SCde"><span class="SFct">setSelection</span>()</spa</a></td>
<td>Set current selection. </td>
</tr>
<tr>
<td><a href="#Gui_EditBox_setText"><span class="SCde"><span class="SFct">setText</span>()</spa</a></td>
<td>Set the editbox content. </td>
</tr>
<tr>
<td><a href="#Gui_EditBox_setTextSilent"><span class="SCde"><span class="SFct">setTextSilent</span>()</spa</a></td>
<td>Set the editbox content, but do not notify change. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_EditBox_IWnd_onFocusEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onFocusEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\editbox.swg#L78" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onFocusEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_FocusEvent">FocusEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_EditBox_IWnd_onKeyEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onKeyEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\editbox.swg#L287" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onKeyEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_KeyEvent">KeyEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_EditBox_IWnd_onMouseEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onMouseEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\editbox.swg#L228" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onMouseEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_MouseEvent">MouseEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_EditBox_IWnd_onPaintEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onPaintEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\editbox.swg#L97" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onPaintEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_PaintEvent">PaintEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_EditBox_IWnd_onTimerEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onTimerEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\editbox.swg#L279" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onTimerEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_TimerEvent">TimerEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_EditBox_create"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">EditBox.</span><span class="api-item-title-strong">create</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\editbox.swg#L795" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">create</span>(parent: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>, content: <span class="STpe">string</span>, position: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span> = {}, id: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span> = <span class="SKwd">null</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_EditBox">EditBox</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_EditBox_deleteSelection"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">EditBox.</span><span class="api-item-title-strong">deleteSelection</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\editbox.swg#L812" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Delete selected text. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">deleteSelection</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_EditBox_getText"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">EditBox.</span><span class="api-item-title-strong">getText</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\editbox.swg#L863" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the associated text. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getText</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="STpe">string</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_EditBox_selectAll"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">EditBox.</span><span class="api-item-title-strong">selectAll</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\editbox.swg#L830" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Select all text. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">selectAll</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_EditBox_setForm"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">EditBox.</span><span class="api-item-title-strong">setForm</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\editbox.swg#L893" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the editbox form. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setForm</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, form: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_EditBoxForm">EditBoxForm</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_EditBox_setInputF64"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">EditBox.</span><span class="api-item-title-strong">setInputF64</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\editbox.swg#L885" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Editbox will edit floating points. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setInputF64</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, min = -<span class="SCst">Swag</span>.<span class="SCst">F64</span>.<span class="SCst">Max</span>, max = <span class="SCst">Swag</span>.<span class="SCst">F64</span>.<span class="SCst">Max</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_EditBox_setInputS64"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">EditBox.</span><span class="api-item-title-strong">setInputS64</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\editbox.swg#L869" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Editbox will edit signed integers. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setInputS64</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, min = <span class="SCst">Swag</span>.<span class="SCst">S64</span>.<span class="SCst">Min</span>, max = <span class="SCst">Swag</span>.<span class="SCst">S64</span>.<span class="SCst">Max</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_EditBox_setInputU64"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">EditBox.</span><span class="api-item-title-strong">setInputU64</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\editbox.swg#L877" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Editbox will edit unsigned integers. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setInputU64</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, min = <span class="SNum">0</span>'<span class="STpe">u64</span>, max = <span class="SCst">Swag</span>.<span class="SCst">U64</span>.<span class="SCst">Max</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_EditBox_setNote"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">EditBox.</span><span class="api-item-title-strong">setNote</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\editbox.swg#L836" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the editbox note. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setNote</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, note: <span class="STpe">string</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_EditBox_setSelection"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">EditBox.</span><span class="api-item-title-strong">setSelection</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\editbox.swg#L821" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set current selection. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setSelection</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, start: <span class="STpe">u64</span>, end: <span class="STpe">u64</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_EditBox_setText"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">EditBox.</span><span class="api-item-title-strong">setText</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\editbox.swg#L851" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the editbox content. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setText</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, str: <span class="STpe">string</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_EditBox_setTextSilent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">EditBox.</span><span class="api-item-title-strong">setTextSilent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\editbox.swg#L843" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the editbox content, but do not notify change. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setTextSilent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, str: <span class="STpe">string</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_EditBoxCheckResult"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">EditBoxCheckResult</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\editbox.swg#L20" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">Incomplete</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Valid</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">InvalidDismiss</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">InvalidShow</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_EditBoxFlags"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">EditBoxFlags</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\editbox.swg#L29" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">Zero</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">AutoLoseFocus</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">ReadOnly</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">RightAlign</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_EditBoxForm"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">EditBoxForm</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\editbox.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">Square</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Round</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Flat</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Transparent</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_EditBoxInputMode"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">EditBoxInputMode</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\editbox.swg#L12" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">String</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">S64</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">U64</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">F64</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_EditCtrl"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">EditCtrl</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\editctrl.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> frameWnd</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_FrameWnd">FrameWnd</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">labelSize</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">editSize</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">label</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Label">Label</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">edit</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_EditBox">EditBox</a></span></span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_EditCtrl_create"><span class="SCde"><span class="SFct">create</span>()</spa</a></td>
<td>Create the popup list, but do not display it. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_EditCtrl_IWnd_onResizeEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onResizeEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\editctrl.swg#L16" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onResizeEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ResizeEvent">ResizeEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_EditCtrl_create"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">EditCtrl.</span><span class="api-item-title-strong">create</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\editctrl.swg#L32" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Create the popup list, but do not display it. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">create</span>(parent: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>, name: <span class="STpe">string</span>, text: <span class="STpe">string</span>, position: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span> = {}, id: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span> = <span class="SKwd">null</span>, flags: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_EditBoxFlags">EditBoxFlags</a></span> = <span class="SCst">Zero</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_EditCtrl">EditCtrl</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_EditDlg"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">EditDlg</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\editdlg.swg#L3" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> dialog</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Dialog">Dialog</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">label</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Label">Label</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">edit</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_EditBox">EditBox</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">richEdit</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditCtrl">RichEditCtrl</a></span></span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_EditDlg_createMultiLine"><span class="SCde"><span class="SFct">createMultiLine</span>()</spa</a></td>
<td>Creates the message box. </td>
</tr>
<tr>
<td><a href="#Gui_EditDlg_createSingleLine"><span class="SCde"><span class="SFct">createSingleLine</span>()</spa</a></td>
<td>Creates the message box. </td>
</tr>
<tr>
<td><a href="#Gui_EditDlg_getText"><span class="SCde"><span class="SFct">getText</span>()</spa</a></td>
<td>Get the text. </td>
</tr>
<tr>
<td><a href="#Gui_EditDlg_setMessage"><span class="SCde"><span class="SFct">setMessage</span>()</spa</a></td>
<td>Associate a message above the editbox. </td>
</tr>
<tr>
<td><a href="#Gui_EditDlg_setText"><span class="SCde"><span class="SFct">setText</span>()</spa</a></td>
<td>Set the text. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_EditDlg_createMultiLine"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">EditDlg.</span><span class="api-item-title-strong">createMultiLine</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\editdlg.swg#L38" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Creates the message box. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">createMultiLine</span>(from: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Surface">Surface</a></span>, heightEdit = <span class="SNum">150</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_EditDlg">EditDlg</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_EditDlg_createSingleLine"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">EditDlg.</span><span class="api-item-title-strong">createSingleLine</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\editdlg.swg#L14" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Creates the message box. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">createSingleLine</span>(from: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Surface">Surface</a></span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_EditDlg">EditDlg</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_EditDlg_getText"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">EditDlg.</span><span class="api-item-title-strong">getText</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\editdlg.swg#L76" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the text. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getText</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="SCst">Core</span>.<span class="SCst">String</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_EditDlg_setMessage"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">EditDlg.</span><span class="api-item-title-strong">setMessage</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\editdlg.swg#L84" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Associate a message above the editbox. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setMessage</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, message: <span class="STpe">string</span>, height: <span class="STpe">s32</span> = <span class="SNum">40</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_EditDlg_setText"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">EditDlg.</span><span class="api-item-title-strong">setText</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\editdlg.swg#L67" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the text. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setText</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, text: <span class="STpe">string</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_EmbInfoCtrl"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">EmbInfoCtrl</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\embinfoctrl.swg#L10" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> frameWnd</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_FrameWnd">FrameWnd</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">label</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Label">Label</a></span></span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_EmbInfoCtrl_create"><span class="SCde"><span class="SFct">create</span>()</spa</a></td>
<td>Create the popup list, but do not display it. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_EmbInfoCtrl_create"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">EmbInfoCtrl.</span><span class="api-item-title-strong">create</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\embinfoctrl.swg#L19" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Create the popup list, but do not display it. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">create</span>(parent: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>, name: <span class="STpe">string</span>, kind: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_EmbInfoCtrlKind">EmbInfoCtrlKind</a></span>, height = <span class="SNum">40</span>, top = <span class="SKwd">true</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_EmbInfoCtrl">EmbInfoCtrl</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_EmbInfoCtrlKind"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">EmbInfoCtrlKind</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\embinfoctrl.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">Critical</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Information</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Event"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">Event</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\events.swg#L43" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde">kind</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_EventKind">EventKind</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">type</span></td>
<td class="code-type"><span class="SCde"><span class="SKwd">const</span> *<span class="SCst">Swag</span>.<span class="SCst">TypeInfo</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">target</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">accepted</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_Event_create"><span class="SCde"><span class="SFct">create</span>()</span></a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Event_create"><span class="SCde"><span class="SFct">create</span>(<span class="SCst">EventKind</span>)</span></a></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Event_create"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Event.</span><span class="api-item-title-strong">create</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\events.swg#L53" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span>(<span class="SCst">T</span>) <span class="SFct">create</span>()-&gt;*<span class="SCst">T</span>
<span class="SKwd">func</span>(<span class="SCst">T</span>) <span class="SFct">create</span>(kind: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_EventKind">EventKind</a></span>)-&gt;*<span class="SCst">T</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_EventKind"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">EventKind</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\events.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">None</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Create</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Destroy</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Resize</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Move</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">PrePaint</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Paint</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">PostPaint</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">KeyPressed</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">KeyReleased</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Rune</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">MouseAccept</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">MousePressed</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">MouseReleased</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">MouseDoubleClick</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">MouseMove</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">MouseEnter</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">MouseLeave</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">MouseWheel</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Quit</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">SysCommand</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">SetTheme</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Timer</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">SetFocus</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">KillFocus</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Command</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">ComputeCommandState</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">ApplyCommandState</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Frame</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">SerializeState</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Show</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Hide</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Invalidate</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">SysUser</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Notify</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_FileDlg"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">FileDlg</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\filedlg.swg#L31" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> dialog</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Dialog">Dialog</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">treeCtrl</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ListCtrl">ListCtrl</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">treePathStr</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'(<span class="SCst">Core</span>.<span class="SCst">String</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">listCtrl</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ListCtrl">ListCtrl</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">comboPath</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ComboBox">ComboBox</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">comboPathStr</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'(<span class="SCst">Core</span>.<span class="SCst">String</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">editFile</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_EditBox">EditBox</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">comboType</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ComboBox">ComboBox</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">dlgOptions</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_FileDlgOptions">FileDlgOptions</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">curFolder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">String</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">curList</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'(<span class="SCst">Core</span>.<span class="SCst">File</span>.<span class="SCst">FileInfo</span>)</span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_FileDlg_create"><span class="SCde"><span class="SFct">create</span>()</spa</a></td>
<td>Creates the message box. </td>
</tr>
<tr>
<td><a href="#Gui_FileDlg_getSelectedName"><span class="SCde"><span class="SFct">getSelectedName</span>()</spa</a></td>
<td>Get the first selection. </td>
</tr>
<tr>
<td><a href="#Gui_FileDlg_getSelectedNames"><span class="SCde"><span class="SFct">getSelectedNames</span>()</spa</a></td>
<td>Get all selections. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_FileDlg_IWnd_onDestroyEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onDestroyEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\filedlg.swg#L50" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onDestroyEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_DestroyEvent">DestroyEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_FileDlg_create"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">FileDlg.</span><span class="api-item-title-strong">create</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\filedlg.swg#L204" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Creates the message box. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">create</span>(from: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Surface">Surface</a></span>, opt: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_FileDlgOptions">FileDlgOptions</a></span> = {})-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_FileDlg">FileDlg</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_FileDlg_getSelectedName"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">FileDlg.</span><span class="api-item-title-strong">getSelectedName</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\filedlg.swg#L471" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the first selection. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getSelectedName</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="SCst">Core</span>.<span class="SCst">String</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_FileDlg_getSelectedNames"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">FileDlg.</span><span class="api-item-title-strong">getSelectedNames</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\filedlg.swg#L480" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get all selections. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getSelectedNames</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="SCst">Core</span>.<span class="SCst">Array</span>'(<span class="SCst">Core</span>.<span class="SCst">String</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_FileDlgMode"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">FileDlgMode</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\filedlg.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">CreateFile</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">SelectOneFile</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">SelectMultiFiles</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">SelectFolder</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_FileDlgOptions"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">FileDlgOptions</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\filedlg.swg#L18" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde">title</span></td>
<td class="code-type"><span class="STpe">string</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnOkName</span></td>
<td class="code-type"><span class="STpe">string</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">openFolder</span></td>
<td class="code-type"><span class="STpe">string</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">editName</span></td>
<td class="code-type"><span class="STpe">string</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">drivePane</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">mode</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_FileDlgMode">FileDlgMode</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">filters</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'({name: <span class="STpe">string</span>, extensions: <span class="STpe">string</span>})</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">shortcuts</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'({type: <span class="SCst">Core</span>.<span class="SCst">Env</span>.<span class="SCst">SpecialDirectory</span>, name: <span class="STpe">string</span>})</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">state</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_FileDlgState">FileDlgState</a></span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_FileDlgState"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">FileDlgState</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\filedlg.swg#L12" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde">position</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">curFolder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">String</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_FocusEvent"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">FocusEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\events.swg#L175" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> base</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Event">Event</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">other</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_FocusStategy"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">FocusStategy</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L36" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">None</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">MousePressed</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_FrameEvent"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">FrameEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\events.swg#L113" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> base</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Event">Event</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">firstFrame</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_FrameWnd"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">FrameWnd</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\framewnd.swg#L29" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> wnd</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">view</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">frameForm</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_FrameWndForm">FrameWndForm</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">frameFlags</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_FrameWndFlags">FrameWndFlags</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">anchor</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_FrameWndAnchor">FrameWndAnchor</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">anchorPos</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">usedColorBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_FrameWnd_createView"><span class="SCde"><span class="SFct">createView</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_FrameWnd_setFrameFlags"><span class="SCde"><span class="SFct">setFrameFlags</span>()</spa</a></td>
<td>Set the frame wnd flags. </td>
</tr>
<tr>
<td><a href="#Gui_FrameWnd_setFrameForm"><span class="SCde"><span class="SFct">setFrameForm</span>()</spa</a></td>
<td>Set the frame wnd form. </td>
</tr>
<tr>
<td><a href="#Gui_FrameWnd_setView"><span class="SCde"><span class="SFct">setView</span>()</spa</a></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_FrameWnd_IWnd_onPaintEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onPaintEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\framewnd.swg#L60" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onPaintEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_PaintEvent">PaintEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_FrameWnd_IWnd_onPostPaintEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onPostPaintEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\framewnd.swg#L106" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onPostPaintEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_PaintEvent">PaintEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_FrameWnd_IWnd_onPrePaintEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onPrePaintEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\framewnd.swg#L43" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onPrePaintEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_PaintEvent">PaintEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_FrameWnd_IWnd_onResizeEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onResizeEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\framewnd.swg#L207" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onResizeEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ResizeEvent">ResizeEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_FrameWnd_createView"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">FrameWnd.</span><span class="api-item-title-strong">createView</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\framewnd.swg#L220" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span>(<span class="SCst">T</span>) <span class="SFct">createView</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, hook: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_HookEvent">HookEvent</a></span> = <span class="SKwd">null</span>)-&gt;*<span class="SCst">T</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_FrameWnd_setFrameFlags"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">FrameWnd.</span><span class="api-item-title-strong">setFrameFlags</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\framewnd.swg#L238" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the frame wnd flags. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setFrameFlags</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, add = <span class="SCst"><a href="#Gui_FrameWndFlags">FrameWndFlags</a></span>.<span class="SCst">Zero</span>, remove = <span class="SCst"><a href="#Gui_FrameWndFlags">FrameWndFlags</a></span>.<span class="SCst">Zero</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_FrameWnd_setFrameForm"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">FrameWnd.</span><span class="api-item-title-strong">setFrameForm</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\framewnd.swg#L255" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the frame wnd form. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setFrameForm</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, form: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_FrameWndForm">FrameWndForm</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_FrameWnd_setView"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">FrameWnd.</span><span class="api-item-title-strong">setView</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\framewnd.swg#L228" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setView</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, what: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_FrameWndAnchor"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">FrameWndAnchor</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\framewnd.swg#L20" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">None</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Left</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Top</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Right</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Bottom</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_FrameWndFlags"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">FrameWndFlags</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\framewnd.swg#L12" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">Zero</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Borders</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">FocusBorder</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">SmallShadow</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_FrameWndForm"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">FrameWndForm</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\framewnd.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">Transparent</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Square</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Round</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_GridLayoutCtrl"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">GridLayoutCtrl</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\gridlayoutctrl.swg#L13" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> wnd</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">gridLayoutFlags</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_GridLayoutCtrlFlags">GridLayoutCtrlFlags</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">numColumns</span></td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">numRows</span></td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">spacingHorz</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">spacingVert</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">layout</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">colSizes</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'(<span class="STpe">f32</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">rowSizes</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'(<span class="STpe">f32</span>)</span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_GridLayoutCtrl_computeLayout"><span class="SCde"><span class="SFct">computeLayout</span>()</spa</a></td>
<td>Recompute layout of all childs. </td>
</tr>
<tr>
<td><a href="#Gui_GridLayoutCtrl_create"><span class="SCde"><span class="SFct">create</span>()</spa</a></td>
<td>Create a layout control. </td>
</tr>
<tr>
<td><a href="#Gui_GridLayoutCtrl_setColRowChild"><span class="SCde"><span class="SFct">setColRowChild</span>()</spa</a></td>
<td>Set the wnd associated with the given <span class="inline-code">col</span> and <span class="inline-code">row</span>. </td>
</tr>
<tr>
<td><a href="#Gui_GridLayoutCtrl_setColSize"><span class="SCde"><span class="SFct">setColSize</span>()</spa</a></td>
<td>Set size, in pixel, of a given column. </td>
</tr>
<tr>
<td><a href="#Gui_GridLayoutCtrl_setRowSize"><span class="SCde"><span class="SFct">setRowSize</span>()</spa</a></td>
<td>Set size, in pixel, of a given row. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_GridLayoutCtrl_IWnd_onResizeEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onResizeEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\gridlayoutctrl.swg#L30" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onResizeEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ResizeEvent">ResizeEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_GridLayoutCtrl_computeLayout"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">GridLayoutCtrl.</span><span class="api-item-title-strong">computeLayout</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\gridlayoutctrl.swg#L138" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Recompute layout of all childs. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">computeLayout</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_GridLayoutCtrl_create"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">GridLayoutCtrl.</span><span class="api-item-title-strong">create</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\gridlayoutctrl.swg#L120" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Create a layout control. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">create</span>(parent: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>, numColumns: <span class="STpe">s32</span>, numRows: <span class="STpe">s32</span>, position: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span> = {})-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_GridLayoutCtrl">GridLayoutCtrl</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_GridLayoutCtrl_setColRowChild"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">GridLayoutCtrl.</span><span class="api-item-title-strong">setColRowChild</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\gridlayoutctrl.swg#L157" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the wnd associated with the given <span class="inline-code">col</span> and <span class="inline-code">row</span>. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setColRowChild</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, child: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>, col: <span class="STpe">s32</span>, row: <span class="STpe">s32</span>, spanH: <span class="STpe">s32</span> = <span class="SNum">1</span>, spanV: <span class="STpe">s32</span> = <span class="SNum">1</span>)</span></code>
</div>
<p> Window can cover multiple columns and rows. </p>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_GridLayoutCtrl_setColSize"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">GridLayoutCtrl.</span><span class="api-item-title-strong">setColSize</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\gridlayoutctrl.swg#L144" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set size, in pixel, of a given column. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setColSize</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, col: <span class="STpe">s32</span>, size: <span class="STpe">f32</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_GridLayoutCtrl_setRowSize"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">GridLayoutCtrl.</span><span class="api-item-title-strong">setRowSize</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\gridlayoutctrl.swg#L150" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set size, in pixel, of a given row. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setRowSize</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, row: <span class="STpe">s32</span>, size: <span class="STpe">f32</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_GridLayoutCtrlFlags"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">GridLayoutCtrlFlags</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\gridlayoutctrl.swg#L5" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">Zero</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">AdaptSizeToContent</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">AdaptColWidth</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">AdaptRowHeight</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Header"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">Header</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\header.swg#L31" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> wnd</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">form</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_HeaderForm">HeaderForm</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">headerFlags</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_HeaderFlags">HeaderFlags</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">marginItems</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sigLayoutChanged</span></td>
<td class="code-type"><span class="SCde"><span class="SKwd">closure</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Header">Header</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sigClicked</span></td>
<td class="code-type"><span class="SCde"><span class="SKwd">closure</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Header">Header</a></span>, <span class="STpe">u32</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">hotIdx</span></td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">hotSeparator</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">moving</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">clicked</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">correctMoving</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sortColumn</span></td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sortMark</span></td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_Header_addItem"><span class="SCde"><span class="SFct">addItem</span>()</spa</a></td>
<td>Add a new view. </td>
</tr>
<tr>
<td><a href="#Gui_Header_create"><span class="SCde"><span class="SFct">create</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Header_getItem"><span class="SCde"><span class="SFct">getItem</span>()</spa</a></td>
<td>Returns the given item, by index. </td>
</tr>
<tr>
<td><a href="#Gui_Header_getItemByCol"><span class="SCde"><span class="SFct">getItemByCol</span>()</spa</a></td>
<td>Returns the given item, by column. </td>
</tr>
<tr>
<td><a href="#Gui_Header_getItemPosition"><span class="SCde"><span class="SFct">getItemPosition</span>()</spa</a></td>
<td>Get an item position by index. </td>
</tr>
<tr>
<td><a href="#Gui_Header_getItemPositionByCol"><span class="SCde"><span class="SFct">getItemPositionByCol</span>()</spa</a></td>
<td>Get an item position by column. </td>
</tr>
<tr>
<td><a href="#Gui_Header_getItemWidth"><span class="SCde"><span class="SFct">getItemWidth</span>()</spa</a></td>
<td>Get an item width by index. </td>
</tr>
<tr>
<td><a href="#Gui_Header_getItemWidthByCol"><span class="SCde"><span class="SFct">getItemWidthByCol</span>()</spa</a></td>
<td>Get an item width by column. </td>
</tr>
<tr>
<td><a href="#Gui_Header_getTotalItemsWidth"><span class="SCde"><span class="SFct">getTotalItemsWidth</span>()</spa</a></td>
<td>Get the header total width. </td>
</tr>
<tr>
<td><a href="#Gui_Header_setItemWidth"><span class="SCde"><span class="SFct">setItemWidth</span>()</spa</a></td>
<td>Set an item width by index. </td>
</tr>
<tr>
<td><a href="#Gui_Header_setItemWidthByCol"><span class="SCde"><span class="SFct">setItemWidthByCol</span>()</spa</a></td>
<td>Set an item width by column. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Header_IWnd_onMouseEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onMouseEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\header.swg#L158" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onMouseEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_MouseEvent">MouseEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Header_IWnd_onPaintEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onPaintEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\header.swg#L114" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onPaintEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_PaintEvent">PaintEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Header_IWnd_paintItem"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">paintItem</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\header.swg#L53" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">paintItem</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, bc: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_PaintContext">PaintContext</a></span>, item: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_HeaderItem">HeaderItem</a></span>, idx: <span class="STpe">u32</span>, x: <span class="STpe">f32</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Header_addItem"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Header.</span><span class="api-item-title-strong">addItem</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\header.swg#L297" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a new view. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">addItem</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, tabName: <span class="STpe">string</span>, width = <span class="SNum">100</span>'<span class="STpe">f32</span>, icon: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Icon">Icon</a></span> = {})-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_HeaderItem">HeaderItem</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Header_create"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Header.</span><span class="api-item-title-strong">create</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\header.swg#L281" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">create</span>(parent: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>, position: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span> = {}, id: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span> = <span class="SKwd">null</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Header">Header</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Header_getItem"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Header.</span><span class="api-item-title-strong">getItem</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\header.swg#L309" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the given item, by index. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getItem</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, idx: <span class="STpe">u32</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_HeaderItem">HeaderItem</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Header_getItemByCol"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Header.</span><span class="api-item-title-strong">getItemByCol</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\header.swg#L316" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the given item, by column. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getItemByCol</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, column: <span class="STpe">u32</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_HeaderItem">HeaderItem</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Header_getItemPosition"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Header.</span><span class="api-item-title-strong">getItemPosition</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\header.swg#L351" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get an item position by index. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getItemPosition</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, idx: <span class="STpe">u32</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Header_getItemPositionByCol"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Header.</span><span class="api-item-title-strong">getItemPositionByCol</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\header.swg#L358" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get an item position by column. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getItemPositionByCol</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, column: <span class="STpe">u32</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Header_getItemWidth"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Header.</span><span class="api-item-title-strong">getItemWidth</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\header.swg#L330" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get an item width by index. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getItemWidth</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, idx: <span class="STpe">u32</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Header_getItemWidthByCol"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Header.</span><span class="api-item-title-strong">getItemWidthByCol</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\header.swg#L337" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get an item width by column. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getItemWidthByCol</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, column: <span class="STpe">u32</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Header_getTotalItemsWidth"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Header.</span><span class="api-item-title-strong">getTotalItemsWidth</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\header.swg#L375" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the header total width. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getTotalItemsWidth</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Header_setItemWidth"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Header.</span><span class="api-item-title-strong">setItemWidth</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\header.swg#L384" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set an item width by index. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setItemWidth</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, idx: <span class="STpe">u32</span>, width: <span class="STpe">f32</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Header_setItemWidthByCol"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Header.</span><span class="api-item-title-strong">setItemWidthByCol</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\header.swg#L391" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set an item width by column. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setItemWidthByCol</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, column: <span class="STpe">u32</span>, width: <span class="STpe">f32</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_HeaderFlags"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">HeaderFlags</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\header.swg#L25" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">Zero</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Clickable</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_HeaderForm"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">HeaderForm</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\header.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">Transparent</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Flat</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Round</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_HeaderItem"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">HeaderItem</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\header.swg#L11" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde">name</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">RichString</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">icon</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Icon">Icon</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">width</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">minWidth</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">maxWidth</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sizeable</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">clickable</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">iconMargin</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">column</span></td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_IActionUI"><span class="api-item-title-kind">interface</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">IActionUI</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\action.swg#L7" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde">accept</span></td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_IActionUI">IActionUI</a></span>, <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ActionContext">ActionContext</a></span>)-&gt;<span class="STpe">bool</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">update</span></td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_IActionUI">IActionUI</a></span>, <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ActionContext">ActionContext</a></span>, *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_CommandStateEvent">CommandStateEvent</a></span>)-&gt;<span class="STpe">bool</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">execute</span></td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_IActionUI">IActionUI</a></span>, <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ActionContext">ActionContext</a></span>)-&gt;<span class="STpe">bool</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_IRichEditLexer"><span class="api-item-title-kind">interface</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">IRichEditLexer</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditlexer.swg#L1" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde">setup</span></td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_IRichEditLexer">IRichEditLexer</a></span>, *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditCtrl">RichEditCtrl</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">insertRune</span></td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_IRichEditLexer">IRichEditLexer</a></span>, *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditCtrl">RichEditCtrl</a></span>, <span class="STpe">rune</span>)-&gt;<span class="STpe">bool</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">compute</span></td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_IRichEditLexer">IRichEditLexer</a></span>, *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditLine">RichEditLine</a></span>, *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditLine">RichEditLine</a></span>, *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditLine">RichEditLine</a></span>)</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_IWnd"><span class="api-item-title-kind">interface</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">IWnd</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L50" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde">onEvent</span></td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_IWnd">IWnd</a></span>, *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Event">Event</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">onHookEvent</span></td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_IWnd">IWnd</a></span>, *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Event">Event</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">onCreateEvent</span></td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_IWnd">IWnd</a></span>, *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_CreateEvent">CreateEvent</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">onDestroyEvent</span></td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_IWnd">IWnd</a></span>, *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_DestroyEvent">DestroyEvent</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">onStateEvent</span></td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_IWnd">IWnd</a></span>, *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_StateEvent">StateEvent</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">onResizeEvent</span></td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_IWnd">IWnd</a></span>, *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ResizeEvent">ResizeEvent</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">onPrePaintEvent</span></td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_IWnd">IWnd</a></span>, *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_PaintEvent">PaintEvent</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">onPaintEvent</span></td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_IWnd">IWnd</a></span>, *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_PaintEvent">PaintEvent</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">onPostPaintEvent</span></td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_IWnd">IWnd</a></span>, *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_PaintEvent">PaintEvent</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">onKeyEvent</span></td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_IWnd">IWnd</a></span>, *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_KeyEvent">KeyEvent</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">onMouseEvent</span></td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_IWnd">IWnd</a></span>, *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_MouseEvent">MouseEvent</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">onSysCommandEvent</span></td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_IWnd">IWnd</a></span>, *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SysCommandEvent">SysCommandEvent</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">onSysUserEvent</span></td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_IWnd">IWnd</a></span>, *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SysUserEvent">SysUserEvent</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">onSetThemeEvent</span></td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_IWnd">IWnd</a></span>, *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SetThemeEvent">SetThemeEvent</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">onTimerEvent</span></td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_IWnd">IWnd</a></span>, *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_TimerEvent">TimerEvent</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">onFocusEvent</span></td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_IWnd">IWnd</a></span>, *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_FocusEvent">FocusEvent</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">onCommandEvent</span></td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_IWnd">IWnd</a></span>, *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_CommandEvent">CommandEvent</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">onComputeStateEvent</span></td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_IWnd">IWnd</a></span>, *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_CommandStateEvent">CommandStateEvent</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">onApplyStateEvent</span></td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_IWnd">IWnd</a></span>, *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_CommandStateEvent">CommandStateEvent</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">onFrameEvent</span></td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_IWnd">IWnd</a></span>, *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_FrameEvent">FrameEvent</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">onSerializeStateEvent</span></td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_IWnd">IWnd</a></span>, *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SerializeStateEvent">SerializeStateEvent</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">onNotifyEvent</span></td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_IWnd">IWnd</a></span>, *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_NotifyEvent">NotifyEvent</a></span>)</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Icon"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">Icon</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\icon.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde">imageList</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ImageList">ImageList</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">index</span></td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sizeX</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sizeY</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_Icon_clear"><span class="SCde"><span class="SFct">clear</span>()</spa</a></td>
<td>Set icon to invalid. </td>
</tr>
<tr>
<td><a href="#Gui_Icon_from"><span class="SCde"><span class="SFct">from</span>()</spa</a></td>
<td>Initialize the icon from an image list. </td>
</tr>
<tr>
<td><a href="#Gui_Icon_isValid"><span class="SCde"><span class="SFct">isValid</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Icon_paint"><span class="SCde"><span class="SFct">paint</span>()</spa</a></td>
<td>Paint icon at the given position. </td>
</tr>
<tr>
<td><a href="#Gui_Icon_set"><span class="SCde"><span class="SFct">set</span>()</spa</a></td>
<td>Initialize the icon from an image list. </td>
</tr>
</table>
<h3>Special Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_Icon_opEquals"><span class="SCde"><span class="SFct">opEquals</span>()</spa</a></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Icon_clear"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Icon.</span><span class="api-item-title-strong">clear</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\icon.swg#L26" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set icon to invalid. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">clear</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Icon_from"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Icon.</span><span class="api-item-title-strong">from</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\icon.swg#L32" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Initialize the icon from an image list. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">from</span>(imageList: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ImageList">ImageList</a></span>, index: <span class="STpe">s32</span>, sizeX: <span class="STpe">f32</span> = <span class="SNum">0</span>, sizeY: <span class="STpe">f32</span> = <span class="SNum">0</span>)-&gt;<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Icon">Icon</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Icon_isValid"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Icon.</span><span class="api-item-title-strong">isValid</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\icon.swg#L23" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">isValid</span>(<span class="SKwd">using</span> <span class="SKwd">const</span> <span class="SKwd">self</span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Icon_opEquals"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Icon.</span><span class="api-item-title-strong">opEquals</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\icon.swg#L14" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">opEquals</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, other: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Icon">Icon</a></span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Icon_paint"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Icon.</span><span class="api-item-title-strong">paint</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\icon.swg#L53" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Paint icon at the given position. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">paint</span>(<span class="SKwd">using</span> <span class="SKwd">const</span> <span class="SKwd">self</span>, painter: *<span class="SCst">Pixel</span>.<span class="SCst">Painter</span>, x: <span class="STpe">f32</span>, y: <span class="STpe">f32</span>, color: <span class="SCst">Pixel</span>.<span class="SCst">Color</span> = <span class="SCst">Argb</span>.<span class="SCst">White</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Icon_set"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Icon.</span><span class="api-item-title-strong">set</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\icon.swg#L42" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Initialize the icon from an image list. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">set</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, imageList: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ImageList">ImageList</a></span>, index: <span class="STpe">s32</span>, sizeX: <span class="STpe">f32</span> = <span class="SNum">0</span>, sizeY: <span class="STpe">f32</span> = <span class="SNum">0</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_IconBar"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">IconBar</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\iconbar.swg#L11" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> wnd</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">iconBarFlags</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_IconBarFlags">IconBarFlags</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">iconSize</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">iconPadding</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sigUpdateState</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SigArray">SigArray</a></span>'(<span class="SKwd">closure</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_CommandStateEvent">CommandStateEvent</a></span>))</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sigCheckChanged</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SigArray">SigArray</a></span>'(<span class="SKwd">closure</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_IconButton">IconButton</a></span>))</span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_IconBar_addCheckableItem"><span class="SCde"><span class="SFct">addCheckableItem</span>()</spa</a></td>
<td>Add a new checkable button. </td>
</tr>
<tr>
<td><a href="#Gui_IconBar_addCheckableToggleItem"><span class="SCde"><span class="SFct">addCheckableToggleItem</span>()</spa</a></td>
<td>Add a new checkable button. </td>
</tr>
<tr>
<td><a href="#Gui_IconBar_addItem"><span class="SCde"><span class="SFct">addItem</span>()</spa</a></td>
<td>Add a new button. </td>
</tr>
<tr>
<td><a href="#Gui_IconBar_create"><span class="SCde"><span class="SFct">create</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_IconBar_setIconBarFlags"><span class="SCde"><span class="SFct">setIconBarFlags</span>()</spa</a></td>
<td>Change bar flags. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_IconBar_IWnd_onComputeStateEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onComputeStateEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\iconbar.swg#L25" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onComputeStateEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_CommandStateEvent">CommandStateEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_IconBar_addCheckableItem"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IconBar.</span><span class="api-item-title-strong">addCheckableItem</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\iconbar.swg#L89" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a new checkable button. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">addCheckableItem</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, icon: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Icon">Icon</a></span>, btnId: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span> = <span class="SKwd">null</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_IconButton">IconButton</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_IconBar_addCheckableToggleItem"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IconBar.</span><span class="api-item-title-strong">addCheckableToggleItem</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\iconbar.swg#L96" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a new checkable button. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">addCheckableToggleItem</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, icon: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Icon">Icon</a></span>, btnId: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span> = <span class="SKwd">null</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_IconButton">IconButton</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_IconBar_addItem"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IconBar.</span><span class="api-item-title-strong">addItem</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\iconbar.swg#L65" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a new button. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">addItem</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, icon: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Icon">Icon</a></span>, btnId: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span> = <span class="SKwd">null</span>, iconBtnFlags = <span class="SCst"><a href="#Gui_IconButtonFlags">IconButtonFlags</a></span>.<span class="SCst">Zero</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_IconButton">IconButton</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_IconBar_create"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IconBar.</span><span class="api-item-title-strong">create</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\iconbar.swg#L53" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">create</span>(parent: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>, iconSize: <span class="STpe">f32</span>, position: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span> = {}, id: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span> = <span class="SKwd">null</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_IconBar">IconBar</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_IconBar_setIconBarFlags"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IconBar.</span><span class="api-item-title-strong">setIconBarFlags</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\iconbar.swg#L102" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Change bar flags. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setIconBarFlags</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, add = <span class="SCst"><a href="#Gui_IconBarFlags">IconBarFlags</a></span>.<span class="SCst">Zero</span>, remove = <span class="SCst"><a href="#Gui_IconBarFlags">IconBarFlags</a></span>.<span class="SCst">Zero</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_IconBarFlags"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">IconBarFlags</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\iconbar.swg#L5" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">Zero</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Vertical</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_IconButton"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">IconButton</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\iconbutton.swg#L50" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> button</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Button">Button</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">form</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_IconButtonForm">IconButtonForm</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">iconBtnFlags</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_IconButtonFlags">IconButtonFlags</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">checkedForm</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_IconButtonCheckedForm">IconButtonCheckedForm</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">arrowPos</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_IconButtonArrowPos">IconButtonArrowPos</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">textPos</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_IconButtonTextPos">IconButtonTextPos</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sigHidePopup</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SigArray">SigArray</a></span>'(<span class="SKwd">closure</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_IconButton">IconButton</a></span>))</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sigShowPopup</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SigArray">SigArray</a></span>'(<span class="SKwd">closure</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_IconButton">IconButton</a></span>))</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sigCheckChanged</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SigArray">SigArray</a></span>'(<span class="SKwd">closure</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_IconButton">IconButton</a></span>))</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sigPaintIcon</span></td>
<td class="code-type"><span class="SCde"><span class="SKwd">closure</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_IconButton">IconButton</a></span>, *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_PaintContext">PaintContext</a></span>, <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">aniBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_BlendColor">BlendColor</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">isChecked</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">popup</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span></span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_IconButton_create"><span class="SCde"><span class="SFct">create</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_IconButton_setCheck"><span class="SCde"><span class="SFct">setCheck</span>()</spa</a></td>
<td>Set the check state of the button (if the button is checkable). </td>
</tr>
<tr>
<td><a href="#Gui_IconButton_setForm"><span class="SCde"><span class="SFct">setForm</span>()</spa</a></td>
<td>Set the button form. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_IconButton_IWnd_hidePopup"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">hidePopup</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\iconbutton.swg#L72" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">hidePopup</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_IconButton_IWnd_onApplyStateEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onApplyStateEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\iconbutton.swg#L105" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onApplyStateEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_CommandStateEvent">CommandStateEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_IconButton_IWnd_onDestroyEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onDestroyEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\iconbutton.swg#L100" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onDestroyEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_DestroyEvent">DestroyEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_IconButton_IWnd_onHookEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onHookEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\iconbutton.swg#L85" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onHookEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Event">Event</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_IconButton_IWnd_onMouseEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onMouseEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\iconbutton.swg#L430" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onMouseEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_MouseEvent">MouseEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_IconButton_IWnd_onPaintEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onPaintEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\iconbutton.swg#L125" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onPaintEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_PaintEvent">PaintEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_IconButton_create"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IconButton.</span><span class="api-item-title-strong">create</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\iconbutton.swg#L475" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">create</span>(parent: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>, icon: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Icon">Icon</a></span>, position: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span> = {}, id: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span> = <span class="SKwd">null</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_IconButton">IconButton</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_IconButton_setCheck"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IconButton.</span><span class="api-item-title-strong">setCheck</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\iconbutton.swg#L492" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the check state of the button (if the button is checkable). </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setCheck</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, checked: <span class="STpe">bool</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_IconButton_setForm"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IconButton.</span><span class="api-item-title-strong">setForm</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\iconbutton.swg#L504" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the button form. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setForm</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, form: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_IconButtonForm">IconButtonForm</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_IconButtonArrowPos"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">IconButtonArrowPos</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\iconbutton.swg#L21" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">None</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Left</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Top</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Right</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">RightUp</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">RightDown</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Bottom</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_IconButtonCheckedForm"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">IconButtonCheckedForm</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\iconbutton.swg#L40" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">None</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Bottom</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Top</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Left</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Right</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Full</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_IconButtonFlags"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">IconButtonFlags</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\iconbutton.swg#L12" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">Zero</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Checkable</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">CheckableToggle</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Popup</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Center</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_IconButtonForm"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">IconButtonForm</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\iconbutton.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">Square</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Round</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">RoundSquare</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_IconButtonTextPos"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">IconButtonTextPos</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\iconbutton.swg#L32" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">Left</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Top</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Right</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Bottom</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ImageList"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">ImageList</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\imagelist.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde">texture</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Texture</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">totalFrames</span></td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">frameSizeX</span></td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">frameSizeY</span></td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_ImageList_countX"><span class="SCde"><span class="SFct">countX</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_ImageList_countY"><span class="SCde"><span class="SFct">countY</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_ImageList_set"><span class="SCde"><span class="SFct">set</span>()</spa</a></td>
<td>Initialize image list. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ImageList_countX"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ImageList.</span><span class="api-item-title-strong">countX</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\imagelist.swg#L16" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">countX</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="STpe">s32</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ImageList_countY"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ImageList.</span><span class="api-item-title-strong">countY</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\imagelist.swg#L17" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">countY</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="STpe">s32</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ImageList_set"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ImageList.</span><span class="api-item-title-strong">set</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\imagelist.swg#L21" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Initialize image list. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">set</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, texture: <span class="SCst">Pixel</span>.<span class="SCst">Texture</span>, fsx: <span class="STpe">s32</span> = <span class="SNum">0</span>, fsy: <span class="STpe">s32</span> = <span class="SNum">0</span>, cpt: <span class="STpe">s32</span> = <span class="SNum">0</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ImageRect"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">ImageRect</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\imagerect.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> wnd</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">movie</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Movie">Movie</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">textureFrame</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Texture</span></span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_ImageRect_clear"><span class="SCde"><span class="SFct">clear</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_ImageRect_create"><span class="SCde"><span class="SFct">create</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_ImageRect_setFile"><span class="SCde"><span class="SFct">setFile</span>()</spa</a></td>
<td>Load and set movie. </td>
</tr>
<tr>
<td><a href="#Gui_ImageRect_setImage"><span class="SCde"><span class="SFct">setImage</span>(<span class="SKwd">self</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">ImageList</span>)</span></a></td>
<td>Associate image. </td>
</tr>
<tr>
<td><a href="#Gui_ImageRect_setImage"><span class="SCde"><span class="SFct">setImage</span>(<span class="SKwd">self</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Image</span>)</span></a></td>
<td>Associate image. </td>
</tr>
<tr>
<td><a href="#Gui_ImageRect_setImage"><span class="SCde"><span class="SFct">setImage</span>(<span class="SKwd">self</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Texture</span>)</span></a></td>
<td>Associate image. </td>
</tr>
<tr>
<td><a href="#Gui_ImageRect_setMovie"><span class="SCde"><span class="SFct">setMovie</span>()</spa</a></td>
<td>Set movie as an image list. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ImageRect_IWnd_onPaintEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onPaintEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\imagerect.swg#L13" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onPaintEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_PaintEvent">PaintEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ImageRect_clear"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ImageRect.</span><span class="api-item-title-strong">clear</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\imagerect.swg#L59" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">clear</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ImageRect_create"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ImageRect.</span><span class="api-item-title-strong">create</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\imagerect.swg#L52" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">create</span>(parent: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>, position: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span> = {}, id: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span> = <span class="SKwd">null</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ImageRect">ImageRect</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ImageRect_setFile"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ImageRect.</span><span class="api-item-title-strong">setFile</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\imagerect.swg#L108" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Load and set movie. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setFile</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, fileName: <span class="STpe">string</span>) <span class="SKwd">throw</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ImageRect_setImage"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ImageRect.</span><span class="api-item-title-strong">setImage</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\imagerect.swg#L73" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Associate image. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setImage</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, image: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ImageList">ImageList</a></span>)</span></code>
</div>
<p>Associate image. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setImage</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, image: <span class="SCst">Pixel</span>.<span class="SCst">Image</span>)</span></code>
</div>
<p>Associate image. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setImage</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, image: <span class="SCst">Pixel</span>.<span class="SCst">Texture</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ImageRect_setMovie"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ImageRect.</span><span class="api-item-title-strong">setMovie</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\imagerect.swg#L65" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set movie as an image list. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setMovie</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, image: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ImageList">ImageList</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Item"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">Item</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\item.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde">name</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">RichString</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">icon</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Icon">Icon</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">id</span></td>
<td class="code-type"><span class="STpe">string</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">userData0</span></td>
<td class="code-type"><span class="SCde">*<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">userData1</span></td>
<td class="code-type"><span class="SCde">*<span class="STpe">void</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_KeyEvent"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">KeyEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\events.swg#L152" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> base</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Event">Event</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">modifiers</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Input</span>.<span class="SCst">KeyModifiers</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">key</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Input</span>.<span class="SCst">Key</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">char</span></td>
<td class="code-type"><span class="STpe">rune</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_KeyShortcut"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">KeyShortcut</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L42" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde">mdf</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Input</span>.<span class="SCst">KeyModifiers</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">key</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Input</span>.<span class="SCst">Key</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">id</span></td>
<td class="code-type"><span class="STpe">string</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">target</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Label"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">Label</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\label.swg#L13" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> wnd</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">icon</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Icon">Icon</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">iconColor</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">labelFlags</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_LabelFlags">LabelFlags</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">horzAlignmentIcon</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">StringHorzAlignment</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">vertAlignmentIcon</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">StringVertAlignment</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">horzAlignmentText</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">StringHorzAlignment</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">vertAlignmentText</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">StringVertAlignment</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">paddingText</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Vector4</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">paddingIcon</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Vector4</span></span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_Label_create"><span class="SCde"><span class="SFct">create</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Label_getText"><span class="SCde"><span class="SFct">getText</span>()</spa</a></td>
<td>Get the text. </td>
</tr>
<tr>
<td><a href="#Gui_Label_setText"><span class="SCde"><span class="SFct">setText</span>()</spa</a></td>
<td>Change the text. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Label_IWnd_onPaintEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onPaintEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\label.swg#L30" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onPaintEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_PaintEvent">PaintEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Label_create"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Label.</span><span class="api-item-title-strong">create</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\label.swg#L117" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">create</span>(parent: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>, name: <span class="STpe">string</span>, position: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span> = {}, id: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span> = <span class="SKwd">null</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Label">Label</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Label_getText"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Label.</span><span class="api-item-title-strong">getText</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\label.swg#L149" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the text. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getText</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="STpe">string</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Label_setText"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Label.</span><span class="api-item-title-strong">setText</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\label.swg#L141" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Change the text. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setText</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, text: <span class="STpe">string</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_LabelFlags"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">LabelFlags</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\label.swg#L5" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">Zero</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">WordWrap</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">LightText</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">AutoHeight</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ListCtrl"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">ListCtrl</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L56" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> frameWnd</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_FrameWnd">FrameWnd</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">horizontalExtent</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">lineHeight</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">indentWidth</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">iconSize</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">iconMarginFirstCol</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">listFlags</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ListFlags">ListFlags</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">selectionMode</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ListSelectionMode">ListSelectionMode</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">leftTextMargin</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sigSelChanged</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SigArray">SigArray</a></span>'(<span class="SKwd">closure</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ListCtrl">ListCtrl</a></span>))</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sigCheckChanged</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SigArray">SigArray</a></span>'(<span class="SKwd">closure</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ListCtrl">ListCtrl</a></span>))</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sigRightClick</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SigArray">SigArray</a></span>'(<span class="SKwd">closure</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ListCtrl">ListCtrl</a></span>, <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Point</span>))</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sigLeftDoubleClick</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SigArray">SigArray</a></span>'(<span class="SKwd">closure</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ListCtrl">ListCtrl</a></span>, <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Point</span>))</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sigExpand</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SigArray">SigArray</a></span>'(<span class="SKwd">closure</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ListCtrl">ListCtrl</a></span>, *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ListLine">ListLine</a></span>))</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sigCollapse</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SigArray">SigArray</a></span>'(<span class="SKwd">closure</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ListCtrl">ListCtrl</a></span>, *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ListLine">ListLine</a></span>))</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sigKeyPressed</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SigArray">SigArray</a></span>'(<span class="SKwd">closure</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ListCtrl">ListCtrl</a></span>, *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_KeyEvent">KeyEvent</a></span>))</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sigVirtualFill</span></td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ListCtrl">ListCtrl</a></span>, *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ListLine">ListLine</a></span>, <span class="STpe">u32</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sigSort</span></td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ListLine">ListLine</a></span>, *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ListLine">ListLine</a></span>, <span class="STpe">u32</span>, <span class="STpe">bool</span>)-&gt;<span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">toFreeLines</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">ArrayPtr</span>'(<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ListLine">ListLine</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">lines</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ListLine">ListLine</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">idxLineToIdxList</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">HashTable</span>'(<span class="STpe">u32</span>, <span class="STpe">u32</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">selModel</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SelModel">SelModel</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">checkModel</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SelModel">SelModel</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">focusIndex</span></td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">hotIndex</span></td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">scrollWnd</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ScrollWnd">ScrollWnd</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">listView</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ListView">ListView</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">header</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Header">Header</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">virtualCount</span></td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">isVirtual</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_ListCtrl_addColumn"><span class="SCde"><span class="SFct">addColumn</span>()</spa</a></td>
<td>Add one column in a multi columns list. </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_clear"><span class="SCde"><span class="SFct">clear</span>()</spa</a></td>
<td>Clear all lines. </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_collapse"><span class="SCde"><span class="SFct">collapse</span>()</spa</a></td>
<td>Collapse the given line. </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_collapseAll"><span class="SCde"><span class="SFct">collapseAll</span>()</spa</a></td>
<td>Collapse all lines. </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_computeLayout"><span class="SCde"><span class="SFct">computeLayout</span>()</spa</a></td>
<td>Compute global layout, once all lines have been added. </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_createLine"><span class="SCde"><span class="SFct">createLine</span>(<span class="SKwd">self</span>, *<span class="SCst">ListLine</span>)</span></a></td>
<td>Add one new line. </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_createLine"><span class="SCde"><span class="SFct">createLine</span>(<span class="SKwd">self</span>, <span class="STpe">string</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Icon</span>, *<span class="SCst">ListLine</span>)</span></a></td>
<td>Add one line, and set first column. </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_createMultiColumns"><span class="SCde"><span class="SFct">createMultiColumns</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_createSimple"><span class="SCde"><span class="SFct">createSimple</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_ensureVisibleLine"><span class="SCde"><span class="SFct">ensureVisibleLine</span>()</spa</a></td>
<td>Set the corresponding line visible. </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_expand"><span class="SCde"><span class="SFct">expand</span>()</spa</a></td>
<td>Expand the given line. </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_getCheckedLines"><span class="SCde"><span class="SFct">getCheckedLines</span>()</spa</a></td>
<td>Get all the checked lines. </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_getFirstVisibleLineIndex"><span class="SCde"><span class="SFct">getFirstVisibleLineIndex</span>()</spa</a></td>
<td>Returns the first visible line index. </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_getFocusLine"><span class="SCde"><span class="SFct">getFocusLine</span>()</spa</a></td>
<td>Get the line with the keyboard focus. </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_getLastVisibleLineIndex"><span class="SCde"><span class="SFct">getLastVisibleLineIndex</span>()</spa</a></td>
<td>Returns the last visible line index. </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_getLine"><span class="SCde"><span class="SFct">getLine</span>()</spa</a></td>
<td>Get a line by index. </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_getLineCount"><span class="SCde"><span class="SFct">getLineCount</span>()</spa</a></td>
<td>Returns the number of lines in the list. </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_getLineHeight"><span class="SCde"><span class="SFct">getLineHeight</span>()</spa</a></td>
<td>Get the height of one line. </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_getSelectedLine"><span class="SCde"><span class="SFct">getSelectedLine</span>()</spa</a></td>
<td>Returns a selected line  Mostly for singlesel mode. </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_getSelectedLines"><span class="SCde"><span class="SFct">getSelectedLines</span>()</spa</a></td>
<td>Get all the selected lines. </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_isLineSelected"><span class="SCde"><span class="SFct">isLineSelected</span>()</spa</a></td>
<td>Returns. </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_isSelectedLine"><span class="SCde"><span class="SFct">isSelectedLine</span>()</spa</a></td>
<td>Returns true if the line at the given index is selected. </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_selectLine"><span class="SCde"><span class="SFct">selectLine</span>()</spa</a></td>
<td>Select of unselect the given line. </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_selectLines"><span class="SCde"><span class="SFct">selectLines</span>()</spa</a></td>
<td>Select a range of lines. </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_setColumnWidth"><span class="SCde"><span class="SFct">setColumnWidth</span>()</spa</a></td>
<td>Set the column width. </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_setFocus"><span class="SCde"><span class="SFct">setFocus</span>()</spa</a></td>
<td>Set focus to the list. </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_setVirtualCount"><span class="SCde"><span class="SFct">setVirtualCount</span>()</spa</a></td>
<td>Set the number of lines in a virtual list. </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_sort"><span class="SCde"><span class="SFct">sort</span>(<span class="SKwd">self</span>)</span></a></td>
<td>Sort list as before. </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_sort"><span class="SCde"><span class="SFct">sort</span>(<span class="SKwd">self</span>, <span class="STpe">u32</span>, <span class="STpe">bool</span>, <span class="STpe">bool</span>)</span></a></td>
<td>Sort the list by a given column  If <span class="inline-code">persistent</span> is true, then the sorted column will be displayed in the  header (if it exists). </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_toggleExpand"><span class="SCde"><span class="SFct">toggleExpand</span>()</spa</a></td>
<td>Collapse the given line. </td>
</tr>
<tr>
<td><a href="#Gui_ListCtrl_unselectAll"><span class="SCde"><span class="SFct">unselectAll</span>()</spa</a></td>
<td>Unselect all lines. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ListCtrl_addColumn"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ListCtrl.</span><span class="api-item-title-strong">addColumn</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L999" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add one column in a multi columns list. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">addColumn</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, name: <span class="STpe">string</span>, width: <span class="STpe">f32</span> = <span class="SNum">100</span>, icon: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Icon">Icon</a></span> = {})</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ListCtrl_clear"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ListCtrl.</span><span class="api-item-title-strong">clear</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L988" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Clear all lines. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">clear</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ListCtrl_collapse"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ListCtrl.</span><span class="api-item-title-strong">collapse</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L1240" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Collapse the given line. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">collapse</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, line: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ListLine">ListLine</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ListCtrl_collapseAll"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ListCtrl.</span><span class="api-item-title-strong">collapseAll</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L1282" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Collapse all lines. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">collapseAll</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ListCtrl_computeLayout"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ListCtrl.</span><span class="api-item-title-strong">computeLayout</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L920" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Compute global layout, once all lines have been added. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">computeLayout</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ListCtrl_createLine"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ListCtrl.</span><span class="api-item-title-strong">createLine</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L957" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add one new line. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">createLine</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, parentLine: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ListLine">ListLine</a></span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ListLine">ListLine</a></span></span></code>
</div>
<p>Add one line, and set first column. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">createLine</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, name: <span class="STpe">string</span>, icon: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Icon">Icon</a></span> = {}, parentLine: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ListLine">ListLine</a></span> = <span class="SKwd">null</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ListLine">ListLine</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ListCtrl_createMultiColumns"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ListCtrl.</span><span class="api-item-title-strong">createMultiColumns</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L787" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">createMultiColumns</span>(parent: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>, position: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span> = {}, id: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span> = <span class="SKwd">null</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ListCtrl">ListCtrl</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ListCtrl_createSimple"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ListCtrl.</span><span class="api-item-title-strong">createSimple</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L769" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">createSimple</span>(parent: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>, position: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span> = {}, id: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span> = <span class="SKwd">null</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ListCtrl">ListCtrl</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ListCtrl_ensureVisibleLine"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ListCtrl.</span><span class="api-item-title-strong">ensureVisibleLine</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L1050" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the corresponding line visible. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">ensureVisibleLine</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, idx: <span class="STpe">u32</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ListCtrl_expand"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ListCtrl.</span><span class="api-item-title-strong">expand</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L1198" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Expand the given line. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">expand</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, line: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ListLine">ListLine</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ListCtrl_getCheckedLines"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ListCtrl.</span><span class="api-item-title-strong">getCheckedLines</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L1092" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get all the checked lines. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getCheckedLines</span>(<span class="SKwd">using</span> <span class="SKwd">const</span> <span class="SKwd">self</span>)-&gt;<span class="SCst">Core</span>.<span class="SCst">Array</span>'(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ListLine">ListLine</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ListCtrl_getFirstVisibleLineIndex"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ListCtrl.</span><span class="api-item-title-strong">getFirstVisibleLineIndex</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L1022" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the first visible line index. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getFirstVisibleLineIndex</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="STpe">u32</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ListCtrl_getFocusLine"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ListCtrl.</span><span class="api-item-title-strong">getFocusLine</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L1068" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the line with the keyboard focus. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getFocusLine</span>(<span class="SKwd">using</span> <span class="SKwd">const</span> <span class="SKwd">self</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ListLine">ListLine</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ListCtrl_getLastVisibleLineIndex"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ListCtrl.</span><span class="api-item-title-strong">getLastVisibleLineIndex</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L1032" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the last visible line index. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getLastVisibleLineIndex</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="STpe">u32</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ListCtrl_getLine"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ListCtrl.</span><span class="api-item-title-strong">getLine</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L943" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get a line by index. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getLine</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, idx: <span class="STpe">u32</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ListLine">ListLine</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ListCtrl_getLineCount"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ListCtrl.</span><span class="api-item-title-strong">getLineCount</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L949" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the number of lines in the list. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getLineCount</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="STpe">u32</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ListCtrl_getLineHeight"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ListCtrl.</span><span class="api-item-title-strong">getLineHeight</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L937" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the height of one line. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getLineHeight</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ListCtrl_getSelectedLine"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ListCtrl.</span><span class="api-item-title-strong">getSelectedLine</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L1109" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns a selected line  Mostly for singlesel mode. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getSelectedLine</span>(<span class="SKwd">using</span> <span class="SKwd">const</span> <span class="SKwd">self</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ListLine">ListLine</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ListCtrl_getSelectedLines"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ListCtrl.</span><span class="api-item-title-strong">getSelectedLines</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L1076" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get all the selected lines. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getSelectedLines</span>(<span class="SKwd">using</span> <span class="SKwd">const</span> <span class="SKwd">self</span>)-&gt;<span class="SCst">Core</span>.<span class="SCst">Array</span>'(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ListLine">ListLine</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ListCtrl_isLineSelected"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ListCtrl.</span><span class="api-item-title-strong">isLineSelected</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L867" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">isLineSelected</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, idx: <span class="STpe">u32</span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ListCtrl_isSelectedLine"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ListCtrl.</span><span class="api-item-title-strong">isSelectedLine</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L1061" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns true if the line at the given index is selected. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">isSelectedLine</span>(<span class="SKwd">using</span> <span class="SKwd">const</span> <span class="SKwd">self</span>, idx: <span class="STpe">u32</span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ListCtrl_selectLine"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ListCtrl.</span><span class="api-item-title-strong">selectLine</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L878" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Select of unselect the given line. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">selectLine</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, idx: <span class="STpe">u32</span>, select: <span class="STpe">bool</span> = <span class="SKwd">true</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ListCtrl_selectLines"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ListCtrl.</span><span class="api-item-title-strong">selectLines</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L899" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Select a range of lines. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">selectLines</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, start: <span class="STpe">u32</span>, end: <span class="STpe">u32</span>, select: <span class="STpe">bool</span> = <span class="SKwd">true</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ListCtrl_setColumnWidth"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ListCtrl.</span><span class="api-item-title-strong">setColumnWidth</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L1006" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the column width. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setColumnWidth</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, idx: <span class="STpe">u32</span>, width: <span class="STpe">f32</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ListCtrl_setFocus"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ListCtrl.</span><span class="api-item-title-strong">setFocus</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L1313" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set focus to the list. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setFocus</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ListCtrl_setVirtualCount"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ListCtrl.</span><span class="api-item-title-strong">setVirtualCount</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L1013" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the number of lines in a virtual list. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setVirtualCount</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, count: <span class="STpe">u32</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ListCtrl_sort"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ListCtrl.</span><span class="api-item-title-strong">sort</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L1189" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Sort list as before. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">sort</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>Sort the list by a given column  If <span class="inline-code">persistent</span> is true, then the sorted column will be displayed in the  header (if it exists). </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">sort</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, column: <span class="STpe">u32</span>, descentOrder = <span class="SKwd">false</span>, persistent = <span class="SKwd">false</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ListCtrl_toggleExpand"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ListCtrl.</span><span class="api-item-title-strong">toggleExpand</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L1304" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Collapse the given line. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">toggleExpand</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, line: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ListLine">ListLine</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ListCtrl_unselectAll"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ListCtrl.</span><span class="api-item-title-strong">unselectAll</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L857" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Unselect all lines. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">unselectAll</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ListFlags"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">ListFlags</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L12" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">Zero</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">HideSelection</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">AlternateLines</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">HorzLines</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">VertLines</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">ForceMarginExpandMark</span></td>
<td>Force one margin even if no expand mark. </td>
</tr>
<tr>
<td class="code-type"><span class="SCst">NoMouseEmptySel</span></td>
<td>Click does not clear selection if not on an item. </td>
</tr>
<tr>
<td class="code-type"><span class="SCst">HotTrack</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ListItem"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">ListItem</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L24" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde">sortKey</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">String</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">colorBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">colorFg</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">column</span></td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">horzAlign</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">StringHorzAlignment</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ListLine"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">ListLine</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L34" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde">colorBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">colorFg</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">userData0</span></td>
<td class="code-type"><span class="SCde">*<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">userData1</span></td>
<td class="code-type"><span class="SCde">*<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">userData2</span></td>
<td class="code-type"><span class="SCde">*<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">userData3</span></td>
<td class="code-type"><span class="SCde">*<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">leftTextMargin</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">forceExpandMark</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">canCheck</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">separator</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">iconMargin</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">lines</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">ArrayPtr</span>'(<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ListLine">ListLine</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">parent</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ListLine">ListLine</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">index</span></td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">level</span></td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">expanded</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_ListLine_canExpand"><span class="SCde"><span class="SFct">canExpand</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_ListLine_getItem"><span class="SCde"><span class="SFct">getItem</span>()</spa</a></td>
<td>Get an item by index. </td>
</tr>
<tr>
<td><a href="#Gui_ListLine_isParentOf"><span class="SCde"><span class="SFct">isParentOf</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_ListLine_setItem"><span class="SCde"><span class="SFct">setItem</span>()</spa</a></td>
<td>Initialize a given column item. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ListLine_canExpand"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ListLine.</span><span class="api-item-title-strong">canExpand</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L111" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">canExpand</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ListLine_getItem"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ListLine.</span><span class="api-item-title-strong">getItem</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L117" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get an item by index. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getItem</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, column: <span class="STpe">u32</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ListItem">ListItem</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ListLine_isParentOf"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ListLine.</span><span class="api-item-title-strong">isParentOf</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L102" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">isParentOf</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, other: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ListLine">ListLine</a></span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ListLine_setItem"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ListLine.</span><span class="api-item-title-strong">setItem</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L130" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Initialize a given column item. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setItem</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, column: <span class="STpe">u32</span>, name: <span class="STpe">string</span>, icon: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Icon">Icon</a></span> = {})-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ListItem">ListItem</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ListSelectionMode"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">ListSelectionMode</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">None</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Single</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Multi</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ListView"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">ListView</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L93" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> wnd</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">list</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ListCtrl">ListCtrl</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">discardMouseRelease</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ListView_IWnd_onKeyEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onKeyEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L159" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onKeyEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_KeyEvent">KeyEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ListView_IWnd_onMouseEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onMouseEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L304" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onMouseEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_MouseEvent">MouseEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ListView_IWnd_onPaintEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onPaintEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L490" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onPaintEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_PaintEvent">PaintEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ListView_IWnd_onResizeEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onResizeEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\listctrl.swg#L153" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onResizeEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ResizeEvent">ResizeEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_MenuCtrl"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">MenuCtrl</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\menuctrl.swg#L34" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> frameWnd</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_FrameWnd">FrameWnd</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">onValidateResult</span></td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_MenuCtrl">MenuCtrl</a></span>, <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">onComputeItem</span></td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_MenuCtrl">MenuCtrl</a></span>, *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_PopupMenuItem">PopupMenuItem</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">menuCtrlFlags</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_MenuCtrlFlags">MenuCtrlFlags</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">paddingSel</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">hotIdx</span></td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">selectedIdx</span></td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">bar</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">endModal</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">endModalResult</span></td>
<td class="code-type"><span class="STpe">string</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">ownerMenu</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_MenuCtrl">MenuCtrl</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">subVisible</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_MenuCtrl">MenuCtrl</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">timer</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Timer">Timer</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">dirtyLayout</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">paintByFrame</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_MenuCtrl_addItem"><span class="SCde"><span class="SFct">addItem</span>(<span class="SKwd">self</span>, <span class="SCst">WndId</span>)</span></a></td>
<td>Add a new item. </td>
</tr>
<tr>
<td><a href="#Gui_MenuCtrl_addItem"><span class="SCde"><span class="SFct">addItem</span>(<span class="SKwd">self</span>, <span class="STpe">string</span>, <span class="SCst">WndId</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Icon</span>, <span class="STpe">string</span>, <span class="STpe">bool</span>, <span class="STpe">bool</span>)</span></a></td>
<td>Add a new item. </td>
</tr>
<tr>
<td><a href="#Gui_MenuCtrl_addPopup"><span class="SCde"><span class="SFct">addPopup</span>(<span class="SKwd">self</span>, *<span class="SCst">MenuCtrl</span>, <span class="SCst">WndId</span>)</span></a></td>
<td>Add a new popup item. </td>
</tr>
<tr>
<td><a href="#Gui_MenuCtrl_addPopup"><span class="SCde"><span class="SFct">addPopup</span>(<span class="SKwd">self</span>, <span class="STpe">string</span>, *<span class="SCst">MenuCtrl</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Icon</span>, <span class="SCst">WndId</span>)</span></a></td>
<td>Add a new popup item. </td>
</tr>
<tr>
<td><a href="#Gui_MenuCtrl_addSeparator"><span class="SCde"><span class="SFct">addSeparator</span>()</spa</a></td>
<td>Add a separator. </td>
</tr>
<tr>
<td><a href="#Gui_MenuCtrl_computeLayoutPopup"><span class="SCde"><span class="SFct">computeLayoutPopup</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_MenuCtrl_createBar"><span class="SCde"><span class="SFct">createBar</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_MenuCtrl_createPopup"><span class="SCde"><span class="SFct">createPopup</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_MenuCtrl_doModal"><span class="SCde"><span class="SFct">doModal</span>()</spa</a></td>
<td>Make the popup menu modal  Will return the selected id. </td>
</tr>
<tr>
<td><a href="#Gui_MenuCtrl_updateState"><span class="SCde"><span class="SFct">updateState</span>()</spa</a></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_MenuCtrl_IWnd_onMouseEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onMouseEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\menuctrl.swg#L267" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onMouseEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_MouseEvent">MouseEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_MenuCtrl_IWnd_onPaintEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onPaintEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\menuctrl.swg#L251" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onPaintEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_PaintEvent">PaintEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_MenuCtrl_IWnd_onTimerEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onTimerEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\menuctrl.swg#L260" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onTimerEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_TimerEvent">TimerEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_MenuCtrl_addItem"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">MenuCtrl.</span><span class="api-item-title-strong">addItem</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\menuctrl.swg#L666" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a new item. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">addItem</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, id: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span>)</span></code>
</div>
<p>Add a new item. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">addItem</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, name: <span class="STpe">string</span>, id: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span>, icon: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Icon">Icon</a></span> = {}, rightName: <span class="STpe">string</span> = <span class="SKwd">null</span>, disabled = <span class="SKwd">false</span>, checked = <span class="SKwd">false</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_MenuCtrl_addPopup"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">MenuCtrl.</span><span class="api-item-title-strong">addPopup</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\menuctrl.swg#L689" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a new popup item. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">addPopup</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, popup: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_MenuCtrl">MenuCtrl</a></span>, id: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span>)</span></code>
</div>
<p>Add a new popup item. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">addPopup</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, name: <span class="STpe">string</span>, popup: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_MenuCtrl">MenuCtrl</a></span>, icon: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Icon">Icon</a></span> = {}, id: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span> = <span class="SKwd">null</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_MenuCtrl_addSeparator"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">MenuCtrl.</span><span class="api-item-title-strong">addSeparator</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\menuctrl.swg#L711" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a separator. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">addSeparator</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_MenuCtrl_computeLayoutPopup"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">MenuCtrl.</span><span class="api-item-title-strong">computeLayoutPopup</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\menuctrl.swg#L566" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">computeLayoutPopup</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_MenuCtrl_createBar"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">MenuCtrl.</span><span class="api-item-title-strong">createBar</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\menuctrl.swg#L650" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">createBar</span>(parent: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>, pos: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span> = {}, id: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span> = <span class="SKwd">null</span>, owner: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span> = <span class="SKwd">null</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_MenuCtrl">MenuCtrl</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_MenuCtrl_createPopup"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">MenuCtrl.</span><span class="api-item-title-strong">createPopup</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\menuctrl.swg#L637" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">createPopup</span>(parent: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>, id: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span> = <span class="SKwd">null</span>, owner: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span> = <span class="SKwd">null</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_MenuCtrl">MenuCtrl</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_MenuCtrl_doModal"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">MenuCtrl.</span><span class="api-item-title-strong">doModal</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\menuctrl.swg#L721" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Make the popup menu modal  Will return the selected id. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">doModal</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, surfacePos: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Point</span>, pos = <span class="SCst"><a href="#Gui_PopupPos">PopupPos</a></span>.<span class="SCst">TopLeft</span>, autoDestroy: <span class="STpe">bool</span> = <span class="SKwd">false</span>)-&gt;<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_MenuCtrl_updateState"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">MenuCtrl.</span><span class="api-item-title-strong">updateState</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\menuctrl.swg#L395" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">updateState</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_MenuCtrlFlags"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">MenuCtrlFlags</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\menuctrl.swg#L11" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">Zero</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">NoKeyShortcuts</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_MessageDlg"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">MessageDlg</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\messagedlg.swg#L3" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> dialog</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Dialog">Dialog</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">labelIcon</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Label">Label</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">label</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Label">Label</a></span></span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_MessageDlg_confirm"><span class="SCde"><span class="SFct">confirm</span>()</spa</a></td>
<td>Message box to <span class="inline-code">confirm</span> something. </td>
</tr>
<tr>
<td><a href="#Gui_MessageDlg_create"><span class="SCde"><span class="SFct">create</span>()</spa</a></td>
<td>Creates the message box. </td>
</tr>
<tr>
<td><a href="#Gui_MessageDlg_doModal"><span class="SCde"><span class="SFct">doModal</span>()</spa</a></td>
<td>Display the box, centered, and returns the id of the pressed button. </td>
</tr>
<tr>
<td><a href="#Gui_MessageDlg_error"><span class="SCde"><span class="SFct">error</span>()</spa</a></td>
<td>Message box to show an error. </td>
</tr>
<tr>
<td><a href="#Gui_MessageDlg_ok"><span class="SCde"><span class="SFct">ok</span>()</spa</a></td>
<td>Message box with a <span class="inline-code">ok</span>. </td>
</tr>
<tr>
<td><a href="#Gui_MessageDlg_okCancel"><span class="SCde"><span class="SFct">okCancel</span>()</spa</a></td>
<td>Message box with a <span class="inline-code">ok</span> and <span class="inline-code">cancel</span> button. </td>
</tr>
<tr>
<td><a href="#Gui_MessageDlg_setIcon"><span class="SCde"><span class="SFct">setIcon</span>()</spa</a></td>
<td>Set big icon. </td>
</tr>
<tr>
<td><a href="#Gui_MessageDlg_setIconColor"><span class="SCde"><span class="SFct">setIconColor</span>()</spa</a></td>
<td>Set big icon color (icon must has been set before). </td>
</tr>
<tr>
<td><a href="#Gui_MessageDlg_yesNo"><span class="SCde"><span class="SFct">yesNo</span>()</spa</a></td>
<td>Message box with a <span class="inline-code">yes</span> and <span class="inline-code">no</span> button. </td>
</tr>
<tr>
<td><a href="#Gui_MessageDlg_yesNoCancel"><span class="SCde"><span class="SFct">yesNoCancel</span>()</spa</a></td>
<td>Message box with a <span class="inline-code">yes</span> and <span class="inline-code">no</span> button. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_MessageDlg_confirm"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">MessageDlg.</span><span class="api-item-title-strong">confirm</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\messagedlg.swg#L166" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Message box to <span class="inline-code">confirm</span> something. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">confirm</span>(from: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Surface">Surface</a></span>, message: <span class="STpe">string</span>, question: <span class="STpe">string</span> = <span class="SKwd">null</span>)-&gt;<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_MessageDlg_create"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">MessageDlg.</span><span class="api-item-title-strong">create</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\messagedlg.swg#L13" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Creates the message box. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">create</span>(from: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Surface">Surface</a></span>, message: <span class="STpe">string</span>, icon: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Icon">Icon</a></span> = {})-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_MessageDlg">MessageDlg</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_MessageDlg_doModal"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">MessageDlg.</span><span class="api-item-title-strong">doModal</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\messagedlg.swg#L36" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Display the box, centered, and returns the id of the pressed button. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">doModal</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, from: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Surface">Surface</a></span>)-&gt;<span class="STpe">string</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_MessageDlg_error"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">MessageDlg.</span><span class="api-item-title-strong">error</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\messagedlg.swg#L176" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Message box to show an error. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">error</span>(from: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Surface">Surface</a></span>, error: <span class="STpe">string</span>, message: <span class="STpe">string</span> = <span class="SKwd">null</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_MessageDlg_ok"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">MessageDlg.</span><span class="api-item-title-strong">ok</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\messagedlg.swg#L116" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Message box with a <span class="inline-code">ok</span>. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">ok</span>(from: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Surface">Surface</a></span>, message: <span class="STpe">string</span>, icon: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Icon">Icon</a></span> = {})</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_MessageDlg_okCancel"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">MessageDlg.</span><span class="api-item-title-strong">okCancel</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\messagedlg.swg#L127" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Message box with a <span class="inline-code">ok</span> and <span class="inline-code">cancel</span> button. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">okCancel</span>(from: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Surface">Surface</a></span>, message: <span class="STpe">string</span>, defaultId = <span class="SCst"><a href="#Gui_Dialog">Dialog</a></span>.<span class="SCst">BtnCancel</span>, icon: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Icon">Icon</a></span> = {})-&gt;<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_MessageDlg_setIcon"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">MessageDlg.</span><span class="api-item-title-strong">setIcon</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\messagedlg.swg#L96" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set big icon. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setIcon</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, icon: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Icon">Icon</a></span>, color: <span class="SCst">Pixel</span>.<span class="SCst">Color</span> = <span class="SCst">Argb</span>.<span class="SCst">Zero</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_MessageDlg_setIconColor"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">MessageDlg.</span><span class="api-item-title-strong">setIconColor</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\messagedlg.swg#L89" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set big icon color (icon must has been set before). </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setIconColor</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, color: <span class="SCst">Pixel</span>.<span class="SCst">Color</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_MessageDlg_yesNo"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">MessageDlg.</span><span class="api-item-title-strong">yesNo</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\messagedlg.swg#L139" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Message box with a <span class="inline-code">yes</span> and <span class="inline-code">no</span> button. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">yesNo</span>(from: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Surface">Surface</a></span>, message: <span class="STpe">string</span>, title: <span class="STpe">string</span> = <span class="SKwd">null</span>, defaultId = <span class="SCst"><a href="#Gui_Dialog">Dialog</a></span>.<span class="SCst">BtnNo</span>, icon: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Icon">Icon</a></span> = {})-&gt;<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_MessageDlg_yesNoCancel"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">MessageDlg.</span><span class="api-item-title-strong">yesNoCancel</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\dialogs\messagedlg.swg#L153" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Message box with a <span class="inline-code">yes</span> and <span class="inline-code">no</span> button. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">yesNoCancel</span>(from: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Surface">Surface</a></span>, message: <span class="STpe">string</span>, defaultId = <span class="SCst"><a href="#Gui_Dialog">Dialog</a></span>.<span class="SCst">BtnCancel</span>, icon: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Icon">Icon</a></span> = {})-&gt;<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_MouseEvent"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">MouseEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\events.swg#L160" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> base</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Event">Event</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">button</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Input</span>.<span class="SCst">MouseButton</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">surfacePos</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Point</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">move</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Point</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">modifiers</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Input</span>.<span class="SCst">KeyModifiers</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_MoveEvent"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">MoveEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\events.swg#L146" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> base</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Event">Event</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">oldPos</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Point</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Movie"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">Movie</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\movie.swg#L11" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde">enableCache</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">mustLoop</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">inPause</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">playFreq</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sigFrameChanged</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SigArray">SigArray</a></span>'(<span class="SKwd">closure</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Movie">Movie</a></span>))</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">numFrames</span></td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">frameIndex</span></td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">frame</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Image</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">source</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_MovieSource">MovieSource</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">imgList</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ImageList">ImageList</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">gif</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Gif</span>.<span class="SCst">Decoder</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">bytes</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'(<span class="STpe">u8</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">cacheFrames</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'(<span class="SCst">Pixel</span>.<span class="SCst">Image</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">currentTime</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">dirtyFrame</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_Movie_clear"><span class="SCde"><span class="SFct">clear</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Movie_isValid"><span class="SCde"><span class="SFct">isValid</span>()</spa</a></td>
<td>Returns true if the movie has valid content. </td>
</tr>
<tr>
<td><a href="#Gui_Movie_pause"><span class="SCde"><span class="SFct">pause</span>()</spa</a></td>
<td>Set/Reset pause state. </td>
</tr>
<tr>
<td><a href="#Gui_Movie_set"><span class="SCde"><span class="SFct">set</span>(<span class="SKwd">self</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">ImageList</span>)</span></a></td>
<td>Associate an image list. </td>
</tr>
<tr>
<td><a href="#Gui_Movie_set"><span class="SCde"><span class="SFct">set</span>(<span class="SKwd">self</span>, <span class="STpe">string</span>)</span></a></td>
<td>Associate a filename. </td>
</tr>
<tr>
<td><a href="#Gui_Movie_setFrameIndex"><span class="SCde"><span class="SFct">setFrameIndex</span>()</spa</a></td>
<td>Set the current video frame. </td>
</tr>
<tr>
<td><a href="#Gui_Movie_update"><span class="SCde"><span class="SFct">update</span>()</spa</a></td>
<td>Update timing and change frames. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Movie_clear"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Movie.</span><span class="api-item-title-strong">clear</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\movie.swg#L80" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">clear</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Movie_isValid"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Movie.</span><span class="api-item-title-strong">isValid</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\movie.swg#L122" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns true if the movie has valid content. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">isValid</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Movie_pause"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Movie.</span><span class="api-item-title-strong">pause</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\movie.swg#L166" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set/Reset pause state. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">pause</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, state: <span class="STpe">bool</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Movie_set"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Movie.</span><span class="api-item-title-strong">set</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\movie.swg#L92" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Associate an image list. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">set</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, imageList: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ImageList">ImageList</a></span>)</span></code>
</div>
<p>Associate a filename. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">set</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, fileName: <span class="STpe">string</span>) <span class="SKwd">throw</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Movie_setFrameIndex"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Movie.</span><span class="api-item-title-strong">setFrameIndex</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\movie.swg#L139" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the current video frame. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setFrameIndex</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, idx: <span class="STpe">s32</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Movie_update"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Movie.</span><span class="api-item-title-strong">update</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\movie.swg#L172" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Update timing and change frames. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">update</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, dt: <span class="STpe">f32</span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_MovieSource"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">MovieSource</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\movie.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">None</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">SpriteSheet</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Gif</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_NativeCursor"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">NativeCursor</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\cursor.win32.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde">cursor</span></td>
<td class="code-type"><span class="SCde"><span class="SKwd">const</span> *<span class="STpe">void</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_NativeSurface"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">NativeSurface</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.win32.swg#L21" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde">hWnd</span></td>
<td class="code-type"><span class="SCde"><span class="SKwd">const</span> *<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">lastDeadChar</span></td>
<td class="code-type"><span class="STpe">u64</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_NotifyEvent"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">NotifyEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\events.swg#L218" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> base</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Event">Event</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">ntfyKind</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_NotifyEvent">NotifyEvent</a></span>.<span class="SCst">Kind</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">from</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_PaintContext"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">PaintContext</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\paintcontext.swg#L3" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde">painter</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Pixel</span>.<span class="SCst">Painter</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">renderer</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Pixel</span>.<span class="SCst">RenderOgl</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">isDisabled</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_PaintEvent"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">PaintEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\events.swg#L134" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> base</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Event">Event</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">bc</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_PaintContext">PaintContext</a></span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_PaintImage"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">PaintImage</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\paintimage.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_PaintImage_draw"><span class="SCde"><span class="SFct">draw</span>(<span class="SKwd">self</span>, <span class="SKwd">func</span>(*<span class="STpe">void</span>, *<span class="SCst">Application</span>, *<span class="SCst">Painter</span>))</span></a></td>
<td>Draw to an image by calling the closure. </td>
</tr>
<tr>
<td><a href="#Gui_PaintImage_draw"><span class="SCde"><span class="SFct">draw</span>(<span class="STpe">s32</span>, <span class="STpe">s32</span>, <span class="SKwd">func</span>(*<span class="STpe">void</span>, *<span class="SCst">Application</span>, *<span class="SCst">Painter</span>))</span></a></td>
<td>One shot paint to image. </td>
</tr>
<tr>
<td><a href="#Gui_PaintImage_init"><span class="SCde"><span class="SFct">init</span>()</spa</a></td>
<td>First init the PaintImage instance. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_PaintImage_draw"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">PaintImage.</span><span class="api-item-title-strong">draw</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\paintimage.swg#L59" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Draw to an image by calling the closure. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">draw</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, call: <span class="SKwd">closure</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Application">Application</a></span>, *<span class="SCst">Pixel</span>.<span class="SCst">Painter</span>))-&gt;<span class="SCst">Pixel</span>.<span class="SCst">Image</span></span></code>
</div>
<p>One shot paint to image. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">draw</span>(w: <span class="STpe">s32</span>, h: <span class="STpe">s32</span>, call: <span class="SKwd">closure</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Application">Application</a></span>, *<span class="SCst">Pixel</span>.<span class="SCst">Painter</span>))-&gt;<span class="SCst">Pixel</span>.<span class="SCst">Image</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_PaintImage_init"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">PaintImage.</span><span class="api-item-title-strong">init</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\paintimage.swg#L50" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>First init the PaintImage instance. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">init</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, w: <span class="STpe">s32</span>, h: <span class="STpe">s32</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_PalettePicker"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">PalettePicker</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\palettepicker.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> wnd</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">selectedColor</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">pal</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'(<span class="SCst">Pixel</span>.<span class="SCst">Color</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">palPos</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'(<span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">boxSize</span></td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">boxMargin</span></td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sigChanged</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SigArray">SigArray</a></span>'(<span class="SKwd">closure</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_PalettePicker">PalettePicker</a></span>))</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">hotIdx</span></td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_PalettePicker_addColor"><span class="SCde"><span class="SFct">addColor</span>()</spa</a></td>
<td>Add a color. </td>
</tr>
<tr>
<td><a href="#Gui_PalettePicker_addHueColors"><span class="SCde"><span class="SFct">addHueColors</span>()</spa</a></td>
<td>Add a range of changing <span class="inline-code">hue</span>. </td>
</tr>
<tr>
<td><a href="#Gui_PalettePicker_addLumColors"><span class="SCde"><span class="SFct">addLumColors</span>()</spa</a></td>
<td>Add a range of changing <span class="inline-code">luminance</span>. </td>
</tr>
<tr>
<td><a href="#Gui_PalettePicker_addSatColors"><span class="SCde"><span class="SFct">addSatColors</span>()</spa</a></td>
<td>Add a range of changing <span class="inline-code">saturation</span>. </td>
</tr>
<tr>
<td><a href="#Gui_PalettePicker_create"><span class="SCde"><span class="SFct">create</span>()</spa</a></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_PalettePicker_IWnd_onMouseEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onMouseEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\palettepicker.swg#L59" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onMouseEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_MouseEvent">MouseEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_PalettePicker_IWnd_onPaintEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onPaintEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\palettepicker.swg#L18" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onPaintEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_PaintEvent">PaintEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_PalettePicker_addColor"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">PalettePicker.</span><span class="api-item-title-strong">addColor</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\palettepicker.swg#L158" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a color. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">addColor</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, c: <span class="SCst">Pixel</span>.<span class="SCst">Color</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_PalettePicker_addHueColors"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">PalettePicker.</span><span class="api-item-title-strong">addHueColors</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\palettepicker.swg#L112" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a range of changing <span class="inline-code">hue</span>. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">addHueColors</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, numColors: <span class="STpe">s32</span>, sat: <span class="STpe">f32</span>, lum: <span class="STpe">f32</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_PalettePicker_addLumColors"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">PalettePicker.</span><span class="api-item-title-strong">addLumColors</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\palettepicker.swg#L126" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a range of changing <span class="inline-code">luminance</span>. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">addLumColors</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, numColors: <span class="STpe">s32</span>, base: <span class="SCst">Pixel</span>.<span class="SCst">Color</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_PalettePicker_addSatColors"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">PalettePicker.</span><span class="api-item-title-strong">addSatColors</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\palettepicker.swg#L142" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a range of changing <span class="inline-code">saturation</span>. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">addSatColors</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, numColors: <span class="STpe">s32</span>, base: <span class="SCst">Pixel</span>.<span class="SCst">Color</span>, lum: <span class="STpe">f32</span> = <span class="SNum">0.5</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_PalettePicker_create"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">PalettePicker.</span><span class="api-item-title-strong">create</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\palettepicker.swg#L104" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">create</span>(parent: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>, position: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span> = {}, id: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span> = <span class="SKwd">null</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_PalettePicker">PalettePicker</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_PopupListCtrl"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">PopupListCtrl</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\popuplistctrl.swg#L14" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> frameWnd</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_FrameWnd">FrameWnd</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">heightItem</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">heightSeparator</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">minWidthPopup</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">maxHeightPopup</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">simFitX</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">simFitY</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sigSelected</span></td>
<td class="code-type"><span class="SCde"><span class="SKwd">closure</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_PopupListCtrl">PopupListCtrl</a></span>, <span class="STpe">u32</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">selectedIdx</span></td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">scrollWnd</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ScrollWnd">ScrollWnd</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">maxIconSize</span></td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_PopupListCtrl_addItem"><span class="SCde"><span class="SFct">addItem</span>()</spa</a></td>
<td>Add a new item in the list. </td>
</tr>
<tr>
<td><a href="#Gui_PopupListCtrl_addSeparator"><span class="SCde"><span class="SFct">addSeparator</span>()</spa</a></td>
<td>Add a separator. </td>
</tr>
<tr>
<td><a href="#Gui_PopupListCtrl_create"><span class="SCde"><span class="SFct">create</span>()</spa</a></td>
<td>Create the popup list, but do not display it. </td>
</tr>
<tr>
<td><a href="#Gui_PopupListCtrl_show"><span class="SCde"><span class="SFct">show</span>()</spa</a></td>
<td>Show popup. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_PopupListCtrl_addItem"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">PopupListCtrl.</span><span class="api-item-title-strong">addItem</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\popuplistctrl.swg#L250" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a new item in the list. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">addItem</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, name: <span class="STpe">string</span>, id: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span> = <span class="SKwd">null</span>, icon: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Icon">Icon</a></span> = {}, userData0: *<span class="STpe">void</span> = <span class="SKwd">null</span>, userData1: *<span class="STpe">void</span> = <span class="SKwd">null</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_PopupListCtrl_addSeparator"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">PopupListCtrl.</span><span class="api-item-title-strong">addSeparator</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\popuplistctrl.swg#L268" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a separator. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">addSeparator</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_PopupListCtrl_create"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">PopupListCtrl.</span><span class="api-item-title-strong">create</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\popuplistctrl.swg#L214" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Create the popup list, but do not display it. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">create</span>(owner: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_PopupListCtrl">PopupListCtrl</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_PopupListCtrl_show"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">PopupListCtrl.</span><span class="api-item-title-strong">show</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\popuplistctrl.swg#L234" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Show popup. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">show</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, x: <span class="STpe">f32</span>, y: <span class="STpe">f32</span>, width: <span class="STpe">f32</span> = <span class="SNum">10</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_PopupListCtrlItem"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">PopupListCtrlItem</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\popuplistctrl.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> base</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Item">Item</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">isSeparator</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">aniSel</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_BlendColor">BlendColor</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">aniText</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_BlendColor">BlendColor</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">aniCheck</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_BlendColor">BlendColor</a></span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_PopupListView"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">PopupListView</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\popuplistctrl.swg#L31" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> wnd</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">list</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_PopupListCtrl">PopupListCtrl</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">hotIdx</span></td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_PopupListView_IWnd_onFocusEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onFocusEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\popuplistctrl.swg#L40" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onFocusEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_FocusEvent">FocusEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_PopupListView_IWnd_onMouseEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onMouseEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\popuplistctrl.swg#L55" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onMouseEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_MouseEvent">MouseEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_PopupListView_IWnd_onPaintEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onPaintEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\popuplistctrl.swg#L126" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onPaintEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_PaintEvent">PaintEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_PopupMenuItem"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">PopupMenuItem</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\menuctrl.swg#L17" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde">rightName</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">RichString</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">popup</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_MenuCtrl">MenuCtrl</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">pos</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">size</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">separator</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">disabled</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">checked</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">hidden</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">aniSel</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_BlendColor">BlendColor</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">aniText</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_BlendColor">BlendColor</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">aniRightText</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_BlendColor">BlendColor</a></span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_PopupPos"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">PopupPos</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\menuctrl.swg#L3" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">TopLeft</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">AnchorTopCenter</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">AnchorBottomCenter</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ProgressBar"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">ProgressBar</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\progressbar.swg#L11" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> wnd</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">prgBarFlags</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ProgressBarFlags">ProgressBarFlags</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">progression</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_ProgressBar_create"><span class="SCde"><span class="SFct">create</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_ProgressBar_setProgression"><span class="SCde"><span class="SFct">setProgression</span>()</spa</a></td>
<td>Set the progression in [0..1] range. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ProgressBar_IWnd_onPaintEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onPaintEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\progressbar.swg#L21" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onPaintEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_PaintEvent">PaintEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ProgressBar_create"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ProgressBar.</span><span class="api-item-title-strong">create</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\progressbar.swg#L92" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">create</span>(parent: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>, position: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span> = {}, id: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span> = <span class="SKwd">null</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ProgressBar">ProgressBar</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ProgressBar_setProgression"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ProgressBar.</span><span class="api-item-title-strong">setProgression</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\progressbar.swg#L102" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the progression in [0..1] range. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setProgression</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, prg: <span class="STpe">f32</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ProgressBarFlags"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">ProgressBarFlags</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\progressbar.swg#L5" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">Zero</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Infinite</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_PropertyList"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">PropertyList</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\property\propertylist.swg#L22" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> frameWnd</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_FrameWnd">FrameWnd</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">nameMarginBefore</span></td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">nameHeight</span></td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">nameMarginAfter</span></td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">descMarginAfter</span></td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">enumFlagsMarginBefore</span></td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">maxSizeCombo</span></td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">maxSizeEditValue</span></td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sigChanged</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SigArray">SigArray</a></span>'(<span class="SKwd">closure</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_PropertyList">PropertyList</a></span>))</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">font0</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">FontFamily</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">font1</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">FontFamily</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">font2</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">FontFamily</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">scrollWnd</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ScrollWnd">ScrollWnd</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">edView</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_PropertyListView">PropertyListView</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">categs</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'({item: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_PropertyListItem">PropertyListItem</a></span>, lvl: <span class="STpe">s32</span>})</span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_PropertyList_addItem"><span class="SCde"><span class="SFct">addItem</span>()</spa</a></td>
<td>Add a new struct to display. </td>
</tr>
<tr>
<td><a href="#Gui_PropertyList_create"><span class="SCde"><span class="SFct">create</span>()</spa</a></td>
<td>Creates a new PropertyList instance. </td>
</tr>
<tr>
<td><a href="#Gui_PropertyList_refresh"><span class="SCde"><span class="SFct">refresh</span>()</spa</a></td>
<td>Refresh displayed values. </td>
</tr>
<tr>
<td><a href="#Gui_PropertyList_setStringFilter"><span class="SCde"><span class="SFct">setStringFilter</span>()</spa</a></td>
<td>Show hidden content depending on the input filter. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_PropertyList_Category"><span class="api-item-title-kind">attr</span> <span class="api-item-title-light">PropertyList.</span><span class="api-item-title-strong">Category</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\property\propertylist.swg#L7" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="api-additional-infos"><b>Usage</b>: all 
</div>
<div class="code-block"><code><span class="SCde"><span class="SKwd">attr</span> <span class="SCst"><a href="#Gui_PropertyList_Category">Category</a></span>(name: <span class="STpe">string</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_PropertyList_Description"><span class="api-item-title-kind">attr</span> <span class="api-item-title-light">PropertyList.</span><span class="api-item-title-strong">Description</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\property\propertylist.swg#L16" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="api-additional-infos"><b>Usage</b>: all 
</div>
<div class="code-block"><code><span class="SCde"><span class="SKwd">attr</span> <span class="SCst"><a href="#Gui_PropertyList_Description">Description</a></span>(desc: <span class="STpe">string</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_PropertyList_EditSlider"><span class="api-item-title-kind">attr</span> <span class="api-item-title-light">PropertyList.</span><span class="api-item-title-strong">EditSlider</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\property\propertylist.swg#L19" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="api-additional-infos"><b>Usage</b>: all 
</div>
<div class="code-block"><code><span class="SCde"><span class="SKwd">attr</span> <span class="SCst"><a href="#Gui_PropertyList_EditSlider">EditSlider</a></span>(min: <span class="STpe">f32</span>, max: <span class="STpe">f32</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_PropertyList_Name"><span class="api-item-title-kind">attr</span> <span class="api-item-title-light">PropertyList.</span><span class="api-item-title-strong">Name</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\property\propertylist.swg#L13" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="api-additional-infos"><b>Usage</b>: all 
</div>
<div class="code-block"><code><span class="SCde"><span class="SKwd">attr</span> <span class="SCst"><a href="#Gui_PropertyList_Name">Name</a></span>(name: <span class="STpe">string</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_PropertyList_SubCategory"><span class="api-item-title-kind">attr</span> <span class="api-item-title-light">PropertyList.</span><span class="api-item-title-strong">SubCategory</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\property\propertylist.swg#L10" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="api-additional-infos"><b>Usage</b>: all 
</div>
<div class="code-block"><code><span class="SCde"><span class="SKwd">attr</span> <span class="SCst"><a href="#Gui_PropertyList_SubCategory">SubCategory</a></span>(name: <span class="STpe">string</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_PropertyList_addItem"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">PropertyList.</span><span class="api-item-title-strong">addItem</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\property\propertylist.swg#L742" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a new struct to display. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">addItem</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, val: <span class="STpe">any</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_PropertyList_create"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">PropertyList.</span><span class="api-item-title-strong">create</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\property\propertylist.swg#L700" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Creates a new PropertyList instance. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">create</span>(parent: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>, position: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span> = {}, id: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span> = <span class="SKwd">null</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_PropertyList">PropertyList</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_PropertyList_refresh"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">PropertyList.</span><span class="api-item-title-strong">refresh</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\property\propertylist.swg#L754" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Refresh displayed values. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">refresh</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_PropertyList_setStringFilter"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">PropertyList.</span><span class="api-item-title-strong">setStringFilter</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\property\propertylist.swg#L763" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Show hidden content depending on the input filter. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setStringFilter</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, filter: <span class="STpe">string</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_PropertyListCtrl"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">PropertyListCtrl</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\property\propertylistctrl.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> frameWnd</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_FrameWnd">FrameWnd</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">splitter</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SplitterCtrl">SplitterCtrl</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">list</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ListCtrl">ListCtrl</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">prop</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_PropertyList">PropertyList</a></span></span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_PropertyListCtrl_addItem"><span class="SCde"><span class="SFct">addItem</span>()</spa</a></td>
<td>Add a new struct to display. </td>
</tr>
<tr>
<td><a href="#Gui_PropertyListCtrl_create"><span class="SCde"><span class="SFct">create</span>()</spa</a></td>
<td>Creates a new PropertyList instance. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_PropertyListCtrl_addItem"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">PropertyListCtrl.</span><span class="api-item-title-strong">addItem</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\property\propertylistctrl.swg#L77" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a new struct to display. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">addItem</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, val: <span class="STpe">any</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_PropertyListCtrl_create"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">PropertyListCtrl.</span><span class="api-item-title-strong">create</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\property\propertylistctrl.swg#L17" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Creates a new PropertyList instance. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">create</span>(parent: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>, position: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span> = {}, id: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span> = <span class="SKwd">null</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_PropertyListCtrl">PropertyListCtrl</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_PropertyListItem"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">PropertyListItem</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\property\propertylist.swg#L45" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde">filter</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">String</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">line</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">ptrData</span></td>
<td class="code-type"><span class="SCde">*<span class="STpe">u8</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">type</span></td>
<td class="code-type"><span class="SCde"><span class="SKwd">const</span> *<span class="SCst">Swag</span>.<span class="SCst">TypeInfo</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">typeValue</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Swag</span>.<span class="SCst">TypeValue</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">forceShow</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">label</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Label">Label</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">labelDesc</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Label">Label</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">editBox</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_EditBox">EditBox</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">checkBox</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_CheckButton">CheckButton</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">comboBox</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ComboBox">ComboBox</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sliderBox</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SliderCtrl">SliderCtrl</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">onResize</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SigArray">SigArray</a></span>'(<span class="SKwd">closure</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_PropertyListItem">PropertyListItem</a></span>))</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">aniFocusBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_BlendColor">BlendColor</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">aniFocusBorder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_BlendColor">BlendColor</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">extendFocus</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_PropertyListView"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">PropertyListView</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\property\propertylistview.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_PushButton"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">PushButton</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\pushbutton.swg#L12" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> button</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Button">Button</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">form</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_PushButtonForm">PushButtonForm</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">aniBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_BlendColor">BlendColor</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">aniBorder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_BlendColor">BlendColor</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">aniText</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_BlendColor">BlendColor</a></span></span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_PushButton_create"><span class="SCde"><span class="SFct">create</span>()</spa</a></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_PushButton_IWnd_onMouseEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onMouseEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\pushbutton.swg#L167" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onMouseEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_MouseEvent">MouseEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_PushButton_IWnd_onPaintEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onPaintEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\pushbutton.swg#L25" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onPaintEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_PaintEvent">PaintEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_PushButton_create"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">PushButton.</span><span class="api-item-title-strong">create</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\pushbutton.swg#L177" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">create</span>(parent: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>, name: <span class="STpe">string</span>, position: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span> = {}, id: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span> = <span class="SKwd">null</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_PushButton">PushButton</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_PushButtonForm"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">PushButtonForm</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\pushbutton.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">Normal</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Default</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Flat</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Strong</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_QuitEvent"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">QuitEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\events.swg#L169" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> base</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Event">Event</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">quitCode</span></td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RadioButton"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">RadioButton</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\radiobutton.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> button</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Button">Button</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">checked</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">radioButtonFlags</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RadioButtonFlags">RadioButtonFlags</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sigChanged</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SigArray">SigArray</a></span>'(<span class="SKwd">closure</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RadioButton">RadioButton</a></span>))</span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_RadioButton_create"><span class="SCde"><span class="SFct">create</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RadioButton_setChecked"><span class="SCde"><span class="SFct">setChecked</span>()</spa</a></td>
<td>Set the checked state. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RadioButton_IWnd_onMouseEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onMouseEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\radiobutton.swg#L107" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onMouseEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_MouseEvent">MouseEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RadioButton_IWnd_onPaintEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onPaintEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\radiobutton.swg#L21" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onPaintEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_PaintEvent">PaintEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RadioButton_create"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RadioButton.</span><span class="api-item-title-strong">create</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\radiobutton.swg#L121" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">create</span>(parent: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>, name: <span class="STpe">string</span>, position: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span> = {}, id: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span> = <span class="SKwd">null</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RadioButton">RadioButton</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RadioButton_setChecked"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RadioButton.</span><span class="api-item-title-strong">setChecked</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\radiobutton.swg#L133" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the checked state. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setChecked</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, checked: <span class="STpe">bool</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RadioButtonFlags"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">RadioButtonFlags</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\radiobutton.swg#L13" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">Zero</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">RightAlign</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ResizeEvent"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">ResizeEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\events.swg#L140" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> base</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Event">Event</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">oldSize</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Point</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCommand"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">RichEditCommand</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcmd.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">None</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">CursorLeft</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">CursorRight</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">CursorUp</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">CursorDown</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">CursorStartLine</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">CursorEndLine</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">CursorStartFile</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">CursorEndFile</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">CursorWordLeft</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">CursorWordRight</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">CursorPageUp</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">CursorPageDown</span></td>
<td>CMD_CURSOR_MATCH_PAIR. </td>
</tr>
<tr>
<td class="code-type"><span class="SCst">ScrollLineUp</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">ScrollLineDown</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">DeleteLeft</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">DeleteRight</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">DeleteLine</span></td>
<td>CMD_EDIT_DELETE_LINE. </td>
</tr>
<tr>
<td class="code-type"><span class="SCst">EditTabulation</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">EditBackTabulation</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">DeleteWordLeft</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">DeleteWordRight</span></td>
<td>CMD_EDIT_DELETE_LINE_LEFT CMD_EDIT_DELETE_LINE_RIGHT. </td>
</tr>
<tr>
<td class="code-type"><span class="SCst">ToggleOverwrite</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">ClipboardCopy</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">ClipboardPaste</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">ClipboardCut</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Undo</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Redo</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">SelectRuneLeft</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">SelectRuneRight</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">SelectRuneUp</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">SelectRuneDown</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">SelectRectLeft</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">SelectRectRight</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">SelectRectUp</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">SelectRectDown</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">SelectStartLine</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">SelectEndLine</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">SelectStartFile</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">SelectEndFile</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">SelectAllFile</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">SelectWordLeft</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">SelectWordRight</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">SelectPageUp</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">SelectPageDown</span></td>
<td>CMD_SEL_HIDE_LINE CMD_MARKER_BOOKMARK_ADD CMD_MARKER_BOOKMARK_PREV CMD_MARKER_BOOKMARK_NEXT CMD_MARKER_BOOKMARK_FAST CMD_MAKE_UPPER CMD_MAKE_LOWER CMD_COMMENT_CPP CMD_UNCOMMENT_CPP. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">RichEditCtrl</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richedit.swg#L30" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> frameWnd</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_FrameWnd">FrameWnd</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">richEdFlags</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditFlags">RichEditFlags</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">lineSpacing</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">overwriteMode</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">tabSize</span></td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">marginLeft</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">marginRight</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">lexer</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_IRichEditLexer">IRichEditLexer</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">form</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditForm">RichEditForm</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sigIsReadOnly</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SigArray">SigArray</a></span>'(<span class="SKwd">closure</span>(*<span class="STpe">bool</span>))</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sigChanged</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SigArray">SigArray</a></span>'(<span class="SKwd">closure</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditCtrl">RichEditCtrl</a></span>))</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sigModified</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SigArray">SigArray</a></span>'(<span class="SKwd">closure</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditCtrl">RichEditCtrl</a></span>))</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">bkColorModel</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">String</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">styles</span></td>
<td class="code-type"><span class="SCde">[<span class="SNum">256</span>] <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditRuneStyle">RichEditRuneStyle</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">stylesSelect</span></td>
<td class="code-type"><span class="SCde">[<span class="SNum">2</span>] <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditRuneStyle">RichEditRuneStyle</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">cursorPos</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditCursor">RichEditCursor</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">selBeg</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditCursor">RichEditCursor</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">lines</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">ArrayPtr</span>'(<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditLine">RichEditLine</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">freeLines</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">ArrayPtr</span>'(<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditLine">RichEditLine</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">scrollWnd</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ScrollWnd">ScrollWnd</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">edView</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditView">RichEditView</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">selRectangle</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">hasSelRect</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">modified</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">recomputeScroll</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">dirtyFirstLineView</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">forceCaretVisible</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">forceCaretVisibleCenter</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">undoMgr</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">ArrayPtr</span>'(<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditUndo">RichEditUndo</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">toUndo</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">ArrayPtr</span>'(<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditUndo">RichEditUndo</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">mapping</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'(<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditMapping">RichEditMapping</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">currentUndo</span></td>
<td class="code-type"><span class="STpe">u64</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">undoCounter</span></td>
<td class="code-type"><span class="STpe">u64</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">modifiedUndoMarker</span></td>
<td class="code-type"><span class="STpe">u64</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">serialUndo</span></td>
<td class="code-type"><span class="STpe">u64</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">countVisibleLines</span></td>
<td class="code-type"><span class="STpe">u64</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">firstVisibleLineView</span></td>
<td class="code-type"><span class="STpe">u64</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">lastVisibleLineView</span></td>
<td class="code-type"><span class="STpe">u64</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">clipFormat</span></td>
<td class="code-type"><span class="STpe">u64</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">cursorPosUndo</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditCursor">RichEditCursor</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">selBegUndo</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditCursor">RichEditCursor</a></span></span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_RichEditCtrl_addMapping"><span class="SCde"><span class="SFct">addMapping</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_addText"><span class="SCde"><span class="SFct">addText</span>()</spa</a></td>
<td>Append a text with a given style. </td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_canRedo"><span class="SCde"><span class="SFct">canRedo</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_canUndo"><span class="SCde"><span class="SFct">canUndo</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_charPressed"><span class="SCde"><span class="SFct">charPressed</span>()</spa</a></td>
<td>Simulate a keyboard character pressed. </td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_checkSelection"><span class="SCde"><span class="SFct">checkSelection</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_clear"><span class="SCde"><span class="SFct">clear</span>()</spa</a></td>
<td>Clear content. </td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_clearMapping"><span class="SCde"><span class="SFct">clearMapping</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_clearSelection"><span class="SCde"><span class="SFct">clearSelection</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_clearUndo"><span class="SCde"><span class="SFct">clearUndo</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_copyToClipboard"><span class="SCde"><span class="SFct">copyToClipboard</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_create"><span class="SCde"><span class="SFct">create</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_deleteBegLine"><span class="SCde"><span class="SFct">deleteBegLine</span>()</spa</a></td>
<td>Delete a given text at the start of each selected line. </td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_deleteLeft"><span class="SCde"><span class="SFct">deleteLeft</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_deleteLine"><span class="SCde"><span class="SFct">deleteLine</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_deleteRight"><span class="SCde"><span class="SFct">deleteRight</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_deleteSelection"><span class="SCde"><span class="SFct">deleteSelection</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_deleteWordLeft"><span class="SCde"><span class="SFct">deleteWordLeft</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_deleteWordRight"><span class="SCde"><span class="SFct">deleteWordRight</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_ensureCaretIsVisible"><span class="SCde"><span class="SFct">ensureCaretIsVisible</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_ensureCursorIsVisible"><span class="SCde"><span class="SFct">ensureCursorIsVisible</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_evaluateSize"><span class="SCde"><span class="SFct">evaluateSize</span>()</spa</a></td>
<td>Evaluate the rendering size. </td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_executeCommand"><span class="SCde"><span class="SFct">executeCommand</span>()</spa</a></td>
<td>Execute a command. </td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_getBegSelection"><span class="SCde"><span class="SFct">getBegSelection</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_getCaretPos"><span class="SCde"><span class="SFct">getCaretPos</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_getCursorDisplayPos"><span class="SCde"><span class="SFct">getCursorDisplayPos</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_getCursorPosFromPoint"><span class="SCde"><span class="SFct">getCursorPosFromPoint</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_getEndSelection"><span class="SCde"><span class="SFct">getEndSelection</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_getMappingCommand"><span class="SCde"><span class="SFct">getMappingCommand</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_getRune"><span class="SCde"><span class="SFct">getRune</span>()</spa</a></td>
<td>Get the rune at the given cursor pos. </td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_getRuneSize"><span class="SCde"><span class="SFct">getRuneSize</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_getSelectedText"><span class="SCde"><span class="SFct">getSelectedText</span>()</spa</a></td>
<td>Get the selected text. </td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_getTabOffset"><span class="SCde"><span class="SFct">getTabOffset</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_getText"><span class="SCde"><span class="SFct">getText</span>()</spa</a></td>
<td>Get the full text. </td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_getTextAndStyles"><span class="SCde"><span class="SFct">getTextAndStyles</span>()</spa</a></td>
<td>Get the full text with the associated styles per character. </td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_getXExtent"><span class="SCde"><span class="SFct">getXExtent</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_insertBegLine"><span class="SCde"><span class="SFct">insertBegLine</span>()</spa</a></td>
<td>Insert text at the start of each selected line. </td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_insertRune"><span class="SCde"><span class="SFct">insertRune</span>()</spa</a></td>
<td>Insert a rune. </td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_insertText"><span class="SCde"><span class="SFct">insertText</span>(<span class="SKwd">self</span>, <span class="STpe">string</span>)</span></a></td>
<td>Insert a text with a given style. </td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_insertText"><span class="SCde"><span class="SFct">insertText</span>(<span class="SKwd">self</span>, <span class="STpe">string</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Array</span>'(<span class="STpe">u8</span>))</span></a></td>
<td>Insert a text with a style per character. </td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_isReadOnly"><span class="SCde"><span class="SFct">isReadOnly</span>()</spa</a></td>
<td>Returns true if the rich edit is read only. </td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_isSelectionEmpty"><span class="SCde"><span class="SFct">isSelectionEmpty</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_keyPressed"><span class="SCde"><span class="SFct">keyPressed</span>()</spa</a></td>
<td>Simulate a keyboard pressed. </td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_lexAll"><span class="SCde"><span class="SFct">lexAll</span>()</spa</a></td>
<td>Lex the full text. </td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_moveCursorDown"><span class="SCde"><span class="SFct">moveCursorDown</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_moveCursorEndFile"><span class="SCde"><span class="SFct">moveCursorEndFile</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_moveCursorEndLine"><span class="SCde"><span class="SFct">moveCursorEndLine</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_moveCursorLeft"><span class="SCde"><span class="SFct">moveCursorLeft</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_moveCursorPageDown"><span class="SCde"><span class="SFct">moveCursorPageDown</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_moveCursorPageUp"><span class="SCde"><span class="SFct">moveCursorPageUp</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_moveCursorRight"><span class="SCde"><span class="SFct">moveCursorRight</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_moveCursorStartFile"><span class="SCde"><span class="SFct">moveCursorStartFile</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_moveCursorStartLine"><span class="SCde"><span class="SFct">moveCursorStartLine</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_moveCursorUp"><span class="SCde"><span class="SFct">moveCursorUp</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_moveCursorWordLeft"><span class="SCde"><span class="SFct">moveCursorWordLeft</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_moveCursorWordRight"><span class="SCde"><span class="SFct">moveCursorWordRight</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_pasteFromClipboard"><span class="SCde"><span class="SFct">pasteFromClipboard</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_popUndo"><span class="SCde"><span class="SFct">popUndo</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_pushUndo"><span class="SCde"><span class="SFct">pushUndo</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_redo"><span class="SCde"><span class="SFct">redo</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_scrollLineDown"><span class="SCde"><span class="SFct">scrollLineDown</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_scrollLineUp"><span class="SCde"><span class="SFct">scrollLineUp</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_selectAll"><span class="SCde"><span class="SFct">selectAll</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_selectWord"><span class="SCde"><span class="SFct">selectWord</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_setCursorPos"><span class="SCde"><span class="SFct">setCursorPos</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_setDefaultMapping"><span class="SCde"><span class="SFct">setDefaultMapping</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_setFocus"><span class="SCde"><span class="SFct">setFocus</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_setForm"><span class="SCde"><span class="SFct">setForm</span>()</spa</a></td>
<td>Set form. </td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_setLexer"><span class="SCde"><span class="SFct">setLexer</span>()</spa</a></td>
<td>Associate a lexer. </td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_setRectangularSelection"><span class="SCde"><span class="SFct">setRectangularSelection</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_setRichEdFlags"><span class="SCde"><span class="SFct">setRichEdFlags</span>()</spa</a></td>
<td>Set richedit flags. </td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_setSelection"><span class="SCde"><span class="SFct">setSelection</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_setText"><span class="SCde"><span class="SFct">setText</span>()</spa</a></td>
<td>Set the text content. </td>
</tr>
<tr>
<td><a href="#Gui_RichEditCtrl_undo"><span class="SCde"><span class="SFct">undo</span>()</spa</a></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_addMapping"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">addMapping</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcmd.swg#L73" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">addMapping</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, key: <span class="SCst">Core</span>.<span class="SCst">Input</span>.<span class="SCst">Key</span>, mdf: <span class="SCst">Core</span>.<span class="SCst">Input</span>.<span class="SCst">KeyModifiers</span>, cmd: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditCommand">RichEditCommand</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_addText"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">addText</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richedittext.swg#L447" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Append a text with a given style. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">addText</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, text: <span class="STpe">string</span>, txtStyle: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditStyleRef">RichEditStyleRef</a></span> = <span class="SNum">0</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_canRedo"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">canRedo</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditundo.swg#L158" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">canRedo</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_canUndo"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">canUndo</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditundo.swg#L104" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">canUndo</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_charPressed"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">charPressed</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richedit.swg#L346" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Simulate a keyboard character pressed. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">charPressed</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, key: <span class="SCst">Core</span>.<span class="SCst">Input</span>.<span class="SCst">Key</span>, ch: <span class="STpe">rune</span>, mdf: <span class="SCst">Core</span>.<span class="SCst">Input</span>.<span class="SCst">KeyModifiers</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_checkSelection"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">checkSelection</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richesitselection.swg#L154" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">checkSelection</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_clear"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">clear</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richedittext.swg#L390" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Clear content. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">clear</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_clearMapping"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">clearMapping</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcmd.swg#L68" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">clearMapping</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_clearSelection"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">clearSelection</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richesitselection.swg#L94" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">clearSelection</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_clearUndo"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">clearUndo</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditundo.swg#L62" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">clearUndo</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_copyToClipboard"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">copyToClipboard</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richesitselection.swg#L266" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">copyToClipboard</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_create"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">create</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richedit.swg#L242" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">create</span>(parent: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>, position: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span> = {}, id: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span> = <span class="SKwd">null</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditCtrl">RichEditCtrl</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_deleteBegLine"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">deleteBegLine</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richedittext.swg#L690" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Delete a given text at the start of each selected line. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">deleteBegLine</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, txt: <span class="STpe">string</span>, lastLine: <span class="STpe">bool</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_deleteLeft"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">deleteLeft</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richesitselection.swg#L205" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">deleteLeft</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_deleteLine"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">deleteLine</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richesitselection.swg#L299" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">deleteLine</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_deleteRight"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">deleteRight</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richesitselection.swg#L228" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">deleteRight</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_deleteSelection"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">deleteSelection</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richesitselection.swg#L179" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">deleteSelection</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_deleteWordLeft"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">deleteWordLeft</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richesitselection.swg#L241" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">deleteWordLeft</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_deleteWordRight"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">deleteWordRight</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richesitselection.swg#L251" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">deleteWordRight</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_ensureCaretIsVisible"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">ensureCaretIsVisible</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcursor.swg#L91" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">ensureCaretIsVisible</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, center = <span class="SKwd">false</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_ensureCursorIsVisible"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">ensureCursorIsVisible</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcursor.swg#L98" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">ensureCursorIsVisible</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, cp: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditCursor">RichEditCursor</a></span>, center: <span class="STpe">bool</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_evaluateSize"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">evaluateSize</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richedittext.swg#L796" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Evaluate the rendering size. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">evaluateSize</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;{w: <span class="STpe">f32</span>, h: <span class="STpe">f32</span>}</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_executeCommand"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">executeCommand</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcmd.swg#L145" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Execute a command. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">executeCommand</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, cmd: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditCommand">RichEditCommand</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_getBegSelection"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">getBegSelection</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richesitselection.swg#L116" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getBegSelection</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditCursor">RichEditCursor</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_getCaretPos"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">getCaretPos</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcursor.swg#L71" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getCaretPos</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, cp: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditCursor">RichEditCursor</a></span>)-&gt;<span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_getCursorDisplayPos"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">getCursorDisplayPos</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcursor.swg#L31" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getCursorDisplayPos</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, cp: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditCursor">RichEditCursor</a></span>)-&gt;<span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Point</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_getCursorPosFromPoint"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">getCursorPosFromPoint</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcursor.swg#L135" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getCursorPosFromPoint</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, pt: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Point</span>)-&gt;<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditCursor">RichEditCursor</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_getEndSelection"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">getEndSelection</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richesitselection.swg#L135" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getEndSelection</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditCursor">RichEditCursor</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_getMappingCommand"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">getMappingCommand</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcmd.swg#L82" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getMappingCommand</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, key: <span class="SCst">Core</span>.<span class="SCst">Input</span>.<span class="SCst">Key</span>, mdf: <span class="SCst">Core</span>.<span class="SCst">Input</span>.<span class="SCst">KeyModifiers</span>)-&gt;<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditCommand">RichEditCommand</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_getRune"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">getRune</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richedittext.swg#L497" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the rune at the given cursor pos. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getRune</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, cp: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditCursor">RichEditCursor</a></span>)-&gt;{char: <span class="STpe">rune</span>, style: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditStyleRef">RichEditStyleRef</a></span>}</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_getRuneSize"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">getRuneSize</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcursor.swg#L12" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getRuneSize</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, line: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditLine">RichEditLine</a></span>, i: <span class="STpe">u64</span>, x: <span class="STpe">f32</span>)-&gt;<span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Point</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_getSelectedText"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">getSelectedText</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richedittext.swg#L405" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the selected text. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getSelectedText</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="SCst">Core</span>.<span class="SCst">String</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_getTabOffset"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">getTabOffset</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcursor.swg#L49" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getTabOffset</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, x: <span class="STpe">s32</span>, numBlanks: *<span class="STpe">s32</span> = <span class="SKwd">null</span>)-&gt;<span class="STpe">s32</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_getText"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">getText</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richedittext.swg#L413" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the full text. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getText</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="SCst">Core</span>.<span class="SCst">String</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_getTextAndStyles"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">getTextAndStyles</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richedittext.swg#L425" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the full text with the associated styles per character. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getTextAndStyles</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;{text: <span class="SCst">Core</span>.<span class="SCst">Array</span>'(<span class="STpe">rune</span>), styles: <span class="SCst">Core</span>.<span class="SCst">Array</span>'(<span class="STpe">u8</span>)}</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_getXExtent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">getXExtent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcursor.swg#L65" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getXExtent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Point</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_insertBegLine"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">insertBegLine</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richedittext.swg#L637" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Insert text at the start of each selected line. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">insertBegLine</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, txt: <span class="STpe">string</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_insertRune"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">insertRune</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richedittext.swg#L513" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Insert a rune. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">insertRune</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, ch: <span class="STpe">rune</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_insertText"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">insertText</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richedittext.swg#L461" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Insert a text with a given style. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">insertText</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, text: <span class="STpe">string</span>)</span></code>
</div>
<p>Insert a text with a style per character. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">insertText</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, text: <span class="STpe">string</span>, txtStyles: <span class="SCst">Core</span>.<span class="SCst">Array</span>'(<span class="STpe">u8</span>))</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_isReadOnly"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">isReadOnly</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richedit.swg#L338" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns true if the rich edit is read only. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">isReadOnly</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_isSelectionEmpty"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">isSelectionEmpty</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richesitselection.swg#L107" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">isSelectionEmpty</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_keyPressed"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">keyPressed</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richedit.swg#L378" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Simulate a keyboard pressed. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">keyPressed</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, key: <span class="SCst">Core</span>.<span class="SCst">Input</span>.<span class="SCst">Key</span>, mdf: <span class="SCst">Core</span>.<span class="SCst">Input</span>.<span class="SCst">KeyModifiers</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_lexAll"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">lexAll</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richedit.swg#L396" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Lex the full text. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">lexAll</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_moveCursorDown"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">moveCursorDown</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcursor.swg#L306" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">moveCursorDown</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, cp: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditCursor">RichEditCursor</a></span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_moveCursorEndFile"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">moveCursorEndFile</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcursor.swg#L392" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">moveCursorEndFile</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, cp: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditCursor">RichEditCursor</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_moveCursorEndLine"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">moveCursorEndLine</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcursor.swg#L381" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">moveCursorEndLine</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, cp: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditCursor">RichEditCursor</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_moveCursorLeft"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">moveCursorLeft</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcursor.swg#L226" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">moveCursorLeft</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, cp: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditCursor">RichEditCursor</a></span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_moveCursorPageDown"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">moveCursorPageDown</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcursor.swg#L411" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">moveCursorPageDown</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, cp: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditCursor">RichEditCursor</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_moveCursorPageUp"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">moveCursorPageUp</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcursor.swg#L398" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">moveCursorPageUp</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, cp: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditCursor">RichEditCursor</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_moveCursorRight"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">moveCursorRight</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcursor.swg#L245" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">moveCursorRight</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, cp: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditCursor">RichEditCursor</a></span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_moveCursorStartFile"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">moveCursorStartFile</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcursor.swg#L386" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">moveCursorStartFile</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, cp: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditCursor">RichEditCursor</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_moveCursorStartLine"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">moveCursorStartLine</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcursor.swg#L347" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">moveCursorStartLine</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, cp: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditCursor">RichEditCursor</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_moveCursorUp"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">moveCursorUp</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcursor.swg#L264" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">moveCursorUp</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, cp: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditCursor">RichEditCursor</a></span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_moveCursorWordLeft"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">moveCursorWordLeft</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcursor.swg#L424" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">moveCursorWordLeft</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, cp: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditCursor">RichEditCursor</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_moveCursorWordRight"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">moveCursorWordRight</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcursor.swg#L473" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">moveCursorWordRight</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, cp: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditCursor">RichEditCursor</a></span>, withSpace: <span class="STpe">bool</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_pasteFromClipboard"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">pasteFromClipboard</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richesitselection.swg#L279" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">pasteFromClipboard</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_popUndo"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">popUndo</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditundo.swg#L84" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">popUndo</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_pushUndo"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">pushUndo</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditundo.swg#L71" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">pushUndo</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_redo"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">redo</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditundo.swg#L167" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">redo</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_scrollLineDown"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">scrollLineDown</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richesitselection.swg#L392" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">scrollLineDown</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_scrollLineUp"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">scrollLineUp</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richesitselection.swg#L370" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">scrollLineUp</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_selectAll"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">selectAll</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richesitselection.swg#L82" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">selectAll</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_selectWord"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">selectWord</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richesitselection.swg#L316" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">selectWord</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_setCursorPos"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">setCursorPos</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcursor.swg#L220" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setCursorPos</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, cp: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditCursor">RichEditCursor</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_setDefaultMapping"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">setDefaultMapping</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcmd.swg#L93" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setDefaultMapping</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_setFocus"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">setFocus</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richedit.swg#L274" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setFocus</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_setForm"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">setForm</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richedit.swg#L288" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set form. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setForm</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, form: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditForm">RichEditForm</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_setLexer"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">setLexer</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richedit.swg#L389" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Associate a lexer. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setLexer</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, lex: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_IRichEditLexer">IRichEditLexer</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_setRectangularSelection"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">setRectangularSelection</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richesitselection.swg#L89" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setRectangularSelection</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, selRect: <span class="STpe">bool</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_setRichEdFlags"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">setRichEdFlags</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richedit.swg#L280" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set richedit flags. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setRichEdFlags</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, flags: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditFlags">RichEditFlags</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_setSelection"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">setSelection</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richesitselection.swg#L100" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setSelection</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, start: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditCursor">RichEditCursor</a></span>, end: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditCursor">RichEditCursor</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_setText"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">setText</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richedittext.swg#L435" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the text content. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setText</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, text: <span class="STpe">string</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCtrl_undo"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditCtrl.</span><span class="api-item-title-strong">undo</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditundo.swg#L113" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">undo</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditCursor"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">RichEditCursor</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcursor.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde">lineIndex</span></td>
<td class="code-type"><span class="STpe">u64</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">charPosInLine</span></td>
<td class="code-type"><span class="STpe">u64</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditFlags"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">RichEditFlags</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richedit.swg#L5" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">Zero</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">ReadOnly</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">ShowFocusSel</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">CurLineBorder</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">DrawBlanks</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">TabToSpaces</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">WordWrap</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">AutoLoseFocus</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditForm"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">RichEditForm</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richedit.swg#L17" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">RichEditCtrl</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Edit</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditLexerSwag"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">RichEditLexerSwag</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditlexerswag.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde">mapStyles</span></td>
<td class="code-type"><span class="SCde">[<span class="SNum">256</span>] <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditStyleRef">RichEditStyleRef</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">mapWords</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">HashTable</span>'(<span class="STpe">string</span>, <span class="STpe">s32</span>)</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditLexerSwag_IRichEditLexer_compute"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IRichEditLexer.</span><span class="api-item-title-strong">compute</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditlexerswag.swg#L423" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">compute</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, linePrev: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditLine">RichEditLine</a></span>, line: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditLine">RichEditLine</a></span>, lineNext: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditLine">RichEditLine</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditLexerSwag_IRichEditLexer_insertRune"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IRichEditLexer.</span><span class="api-item-title-strong">insertRune</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditlexerswag.swg#L391" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">insertRune</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, ed: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditCtrl">RichEditCtrl</a></span>, ch: <span class="STpe">rune</span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditLexerSwag_IRichEditLexer_setup"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IRichEditLexer.</span><span class="api-item-title-strong">setup</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditlexerswag.swg#L64" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setup</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, ed: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditCtrl">RichEditCtrl</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditLine"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">RichEditLine</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditline.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditMapping"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">RichEditMapping</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richedit.swg#L23" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde">key</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Input</span>.<span class="SCst">Key</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">modifiers</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Input</span>.<span class="SCst">KeyModifiers</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">command</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditCommand">RichEditCommand</a></span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditRuneStyle"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">RichEditRuneStyle</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcharstyle.swg#L6" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde">colBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">colFg</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">fontMode</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">FontFamilyStyle</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">underline</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">UnderlineStyle</span></span></td>
<td></td>
</tr>
</table>
<h3>Special Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_RichEditRuneStyle_opEquals"><span class="SCde"><span class="SFct">opEquals</span>()</spa</a></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditRuneStyle_opEquals"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichEditRuneStyle.</span><span class="api-item-title-strong">opEquals</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditcharstyle.swg#L16" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">opEquals</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, other: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditRuneStyle">RichEditRuneStyle</a></span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditUndo"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">RichEditUndo</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditundo.swg#L13" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde">type</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditUndoType">RichEditUndoType</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">concatChars</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">selBeg</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditCursor">RichEditCursor</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">cursorPos</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditCursor">RichEditCursor</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">runes</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'(<span class="STpe">rune</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">styles</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'(<span class="STpe">u8</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">serial</span></td>
<td class="code-type"><span class="STpe">u64</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">hasSelRect</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">begSelectionBefore</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditCursor">RichEditCursor</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">endSelectionBefore</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditCursor">RichEditCursor</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">begSelectionAfter</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditCursor">RichEditCursor</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">endSelectionAfter</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_RichEditCursor">RichEditCursor</a></span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditUndoType"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">RichEditUndoType</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditundo.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">Delete</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Insert</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Selection</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Show</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Hide</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_RichEditView"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">RichEditView</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\richedit\richeditview.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ScrollWnd"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">ScrollWnd</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\scrollwnd.swg#L13" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> frameWnd</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_FrameWnd">FrameWnd</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">scrollIncrementV</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">scrollIncrementH</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">scrollSize</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Point</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">scrollWndFlags</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ScrollWndFlags">ScrollWndFlags</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sigScrollPosChanged</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SigArray">SigArray</a></span>'(<span class="SKwd">closure</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ScrollWnd">ScrollWnd</a></span>, <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Point</span>, <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Point</span>))</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sigScrollSizeChanged</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SigArray">SigArray</a></span>'(<span class="SKwd">closure</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ScrollWnd">ScrollWnd</a></span>, <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Point</span>, <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Point</span>))</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">oldMouseCapture</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">scrollSmoothPos</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Point</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">scrollRequestPos</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Point</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">smoothScroll</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">showBarV</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">needV</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">posBoxV</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sizeBoxV</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">isHotV</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">isHotBarV</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">isPressedV</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">showBarH</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">needH</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">posBoxH</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sizeBoxH</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">isHotH</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">isHotBarH</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">isPressedH</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">isMoving</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">isGrabbing</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">correcMoving</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">offsetClientV</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">offsetClientH</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">paddingZ</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">paddingW</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">aniBkBarV</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_BlendColor">BlendColor</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">aniBkBarH</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_BlendColor">BlendColor</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">aniBoxV</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_BlendColor">BlendColor</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">aniBoxH</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_BlendColor">BlendColor</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">grabbingPos</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Point</span></span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_ScrollWnd_getClientScrollRect"><span class="SCde"><span class="SFct">getClientScrollRect</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_ScrollWnd_getScrollPos"><span class="SCde"><span class="SFct">getScrollPos</span>()</spa</a></td>
<td>Get the current scroll position. </td>
</tr>
<tr>
<td><a href="#Gui_ScrollWnd_setScrollPos"><span class="SCde"><span class="SFct">setScrollPos</span>(<span class="SKwd">self</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Point</span>)</span></a></td>
<td>Set the current scroll position. </td>
</tr>
<tr>
<td><a href="#Gui_ScrollWnd_setScrollPos"><span class="SCde"><span class="SFct">setScrollPos</span>(<span class="SKwd">self</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>)</span></a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_ScrollWnd_setScrollSize"><span class="SCde"><span class="SFct">setScrollSize</span>()</spa</a></td>
<td>Set the scroll size (window virtual size). </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ScrollWnd_IWnd_onMouseEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onMouseEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\scrollwnd.swg#L78" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onMouseEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_MouseEvent">MouseEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ScrollWnd_IWnd_onPaintEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onPaintEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\scrollwnd.swg#L197" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onPaintEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_PaintEvent">PaintEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ScrollWnd_IWnd_onResizeEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onResizeEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\scrollwnd.swg#L66" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onResizeEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ResizeEvent">ResizeEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ScrollWnd_getClientScrollRect"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ScrollWnd.</span><span class="api-item-title-strong">getClientScrollRect</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\scrollwnd.swg#L514" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getClientScrollRect</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ScrollWnd_getScrollPos"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ScrollWnd.</span><span class="api-item-title-strong">getScrollPos</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\scrollwnd.swg#L454" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the current scroll position. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getScrollPos</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Point</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ScrollWnd_setScrollPos"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ScrollWnd.</span><span class="api-item-title-strong">setScrollPos</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\scrollwnd.swg#L489" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the current scroll position. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setScrollPos</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, pt: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Point</span>)</span></code>
</div>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setScrollPos</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, x: <span class="STpe">f32</span>, y: <span class="STpe">f32</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ScrollWnd_setScrollSize"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ScrollWnd.</span><span class="api-item-title-strong">setScrollSize</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\scrollwnd.swg#L502" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the scroll size (window virtual size). </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setScrollSize</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, x: <span class="STpe">f32</span>, y: <span class="STpe">f32</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ScrollWndFlags"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">ScrollWndFlags</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\scrollwnd.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">Zero</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">SmallBar</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">ForceHorizontal</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">ForceVertical</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">TransparentBar</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_SelModel"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">SelModel</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\selmodel.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde">selection</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'({start: <span class="STpe">u32</span>, end: <span class="STpe">u32</span>})</span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_SelModel_getSelectedRangeIndex"><span class="SCde"><span class="SFct">getSelectedRangeIndex</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_SelModel_isEmpty"><span class="SCde"><span class="SFct">isEmpty</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_SelModel_isSelected"><span class="SCde"><span class="SFct">isSelected</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_SelModel_select"><span class="SCde"><span class="SFct">select</span>()</spa</a></td>
<td>Select the given index. </td>
</tr>
<tr>
<td><a href="#Gui_SelModel_unselectAll"><span class="SCde"><span class="SFct">unselectAll</span>()</spa</a></td>
<td>Unselect all. </td>
</tr>
</table>
<h3>Special Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_SelModel_opVisit"><span class="SCde"><span class="SFct">opVisit</span>()</spa</a></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_SelModel_getSelectedRangeIndex"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">SelModel.</span><span class="api-item-title-strong">getSelectedRangeIndex</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\selmodel.swg#L11" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getSelectedRangeIndex</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, idx: <span class="STpe">u32</span>)-&gt;<span class="STpe">u32</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_SelModel_isEmpty"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">SelModel.</span><span class="api-item-title-strong">isEmpty</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\selmodel.swg#L24" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">isEmpty</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_SelModel_isSelected"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">SelModel.</span><span class="api-item-title-strong">isSelected</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\selmodel.swg#L25" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">isSelected</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, idx: <span class="STpe">u32</span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_SelModel_opVisit"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">SelModel.</span><span class="api-item-title-strong">opVisit</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\selmodel.swg#L82" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SAtr">#[<a href="swag.runtime.php#Swag_Macro">Swag.Macro</a>]</span>
<span class="SKwd">func</span>(ptr: <span class="STpe">bool</span>, back: <span class="STpe">bool</span>) <span class="SFct">opVisit</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, stmt: <span class="STpe">code</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_SelModel_select"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">SelModel.</span><span class="api-item-title-strong">select</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\selmodel.swg#L35" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Select the given index. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">select</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, idx: <span class="STpe">u32</span>, select: <span class="STpe">bool</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_SelModel_unselectAll"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">SelModel.</span><span class="api-item-title-strong">unselectAll</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\selmodel.swg#L29" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Unselect all. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">unselectAll</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_SerializeStateEvent"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">SerializeStateEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\events.swg#L212" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> base</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Event">Event</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">ser</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Core</span>.<span class="SCst">Serialization</span>.<span class="SCst">Serializer</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_SetThemeEvent"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">SetThemeEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\events.swg#L205" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> base</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Event">Event</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">style</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeStyle">ThemeStyle</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">parent</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeStyle">ThemeStyle</a></span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_SigArray"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">SigArray</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\sigarray.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">struct</span> <span class="SCst"><a href="#Gui_SigArray">SigArray</a></span>(<span class="SCst">T</span>)</span></code>
</div>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde">arr</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'(<span class="SCst">T</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">disabled</span></td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_SigArray_call"><span class="SCde"><span class="SFct">call</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_SigArray_disable"><span class="SCde"><span class="SFct">disable</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_SigArray_enable"><span class="SCde"><span class="SFct">enable</span>()</spa</a></td>
<td></td>
</tr>
</table>
<h3>Special Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_SigArray_opAssign"><span class="SCde"><span class="SFct">opAssign</span>()</spa</a></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_SigArray_call"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">SigArray.</span><span class="api-item-title-strong">call</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\sigarray.swg#L29" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">call</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, params: ...)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_SigArray_disable"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">SigArray.</span><span class="api-item-title-strong">disable</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\sigarray.swg#L18" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">disable</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_SigArray_enable"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">SigArray.</span><span class="api-item-title-strong">enable</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\sigarray.swg#L23" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">enable</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_SigArray_opAssign"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">SigArray.</span><span class="api-item-title-strong">opAssign</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\sigarray.swg#L12" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span>(op: <span class="STpe">string</span>) <span class="SFct">opAssign</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, fn: <span class="SCst">T</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_SizingBorder"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">SizingBorder</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\surfacewnd.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">Zero</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Left</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Top</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Right</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Bottom</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Slider"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">Slider</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\slider.swg#L14" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> wnd</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">value1</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">value2</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">min</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">max</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">steps</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sliderFlags</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SliderFlags">SliderFlags</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sigStartChange</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SigArray">SigArray</a></span>'(<span class="SKwd">closure</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Slider">Slider</a></span>))</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sigChanged</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SigArray">SigArray</a></span>'(<span class="SKwd">closure</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Slider">Slider</a></span>))</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sigEndChange</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SigArray">SigArray</a></span>'(<span class="SKwd">closure</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Slider">Slider</a></span>))</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">isMoving</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">isPressed1</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">isPressed2</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">isHot1</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">isHot2</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">hasValue1</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">hasValue2</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">correcMoving</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">aniMark1</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_BlendColor">BlendColor</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">aniMark2</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_BlendColor">BlendColor</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">aniStep1</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_BlendColor">BlendColor</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">aniStep2</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_BlendColor">BlendColor</a></span></span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_Slider_create"><span class="SCde"><span class="SFct">create</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Slider_getValue"><span class="SCde"><span class="SFct">getValue</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Slider_getValueLeft"><span class="SCde"><span class="SFct">getValueLeft</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Slider_getValueRight"><span class="SCde"><span class="SFct">getValueRight</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Slider_setMinMax"><span class="SCde"><span class="SFct">setMinMax</span>()</spa</a></td>
<td>Set the <span class="inline-code">min</span>, <span class="inline-code">max</span> and <span class="inline-code">steps</span> values. </td>
</tr>
<tr>
<td><a href="#Gui_Slider_setRangeValues"><span class="SCde"><span class="SFct">setRangeValues</span>()</spa</a></td>
<td>Set left and right values, for a range. </td>
</tr>
<tr>
<td><a href="#Gui_Slider_setValue"><span class="SCde"><span class="SFct">setValue</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Slider_setValueLeft"><span class="SCde"><span class="SFct">setValueLeft</span>()</spa</a></td>
<td>Set the left value (in case of a range). </td>
</tr>
<tr>
<td><a href="#Gui_Slider_setValueRight"><span class="SCde"><span class="SFct">setValueRight</span>()</spa</a></td>
<td>Set the right value (in case of a range). </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Slider_IWnd_onMouseEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onMouseEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\slider.swg#L201" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onMouseEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_MouseEvent">MouseEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Slider_IWnd_onPaintEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onPaintEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\slider.swg#L45" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onPaintEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_PaintEvent">PaintEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Slider_create"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Slider.</span><span class="api-item-title-strong">create</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\slider.swg#L506" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">create</span>(parent: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>, position: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span> = {}, id: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span> = <span class="SKwd">null</span>, range = <span class="SKwd">false</span>, vertical = <span class="SKwd">false</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Slider">Slider</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Slider_getValue"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Slider.</span><span class="api-item-title-strong">getValue</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\slider.swg#L461" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getValue</span>(<span class="SKwd">using</span> <span class="SKwd">const</span> <span class="SKwd">self</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Slider_getValueLeft"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Slider.</span><span class="api-item-title-strong">getValueLeft</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\slider.swg#L462" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getValueLeft</span>(<span class="SKwd">using</span> <span class="SKwd">const</span> <span class="SKwd">self</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Slider_getValueRight"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Slider.</span><span class="api-item-title-strong">getValueRight</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\slider.swg#L463" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getValueRight</span>(<span class="SKwd">using</span> <span class="SKwd">const</span> <span class="SKwd">self</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Slider_setMinMax"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Slider.</span><span class="api-item-title-strong">setMinMax</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\slider.swg#L484" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the <span class="inline-code">min</span>, <span class="inline-code">max</span> and <span class="inline-code">steps</span> values. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setMinMax</span>(<span class="SKwd">self</span>, min: <span class="STpe">f32</span>, max: <span class="STpe">f32</span>, steps: <span class="STpe">f32</span> = <span class="SNum">0</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Slider_setRangeValues"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Slider.</span><span class="api-item-title-strong">setRangeValues</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\slider.swg#L494" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set left and right values, for a range. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setRangeValues</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, left: <span class="STpe">f32</span>, right: <span class="STpe">f32</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Slider_setValue"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Slider.</span><span class="api-item-title-strong">setValue</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\slider.swg#L465" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setValue</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, f: <span class="STpe">f32</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Slider_setValueLeft"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Slider.</span><span class="api-item-title-strong">setValueLeft</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\slider.swg#L468" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the left value (in case of a range). </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setValueLeft</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, f: <span class="STpe">f32</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Slider_setValueRight"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Slider.</span><span class="api-item-title-strong">setValueRight</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\slider.swg#L476" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the right value (in case of a range). </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setValueRight</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, f: <span class="STpe">f32</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_SliderCtrl"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">SliderCtrl</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\sliderctrl.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> frameWnd</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_FrameWnd">FrameWnd</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">labelSize</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">editSize</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sigChanged</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SigArray">SigArray</a></span>'(<span class="SKwd">closure</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SliderCtrl">SliderCtrl</a></span>))</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">label</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Label">Label</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">slider</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Slider">Slider</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">edit</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_EditBox">EditBox</a></span></span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_SliderCtrl_create"><span class="SCde"><span class="SFct">create</span>()</spa</a></td>
<td>Create the popup list, but do not display it. </td>
</tr>
<tr>
<td><a href="#Gui_SliderCtrl_setMinMaxF32"><span class="SCde"><span class="SFct">setMinMaxF32</span>()</spa</a></td>
<td>Set a f32 range. </td>
</tr>
<tr>
<td><a href="#Gui_SliderCtrl_setMinMaxS64"><span class="SCde"><span class="SFct">setMinMaxS64</span>()</spa</a></td>
<td>Set a f64 range. </td>
</tr>
<tr>
<td><a href="#Gui_SliderCtrl_setMinMaxU64"><span class="SCde"><span class="SFct">setMinMaxU64</span>()</spa</a></td>
<td>Set a u64 range. </td>
</tr>
<tr>
<td><a href="#Gui_SliderCtrl_setValue"><span class="SCde"><span class="SFct">setValue</span>()</spa</a></td>
<td>Set the slider current value. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_SliderCtrl_IWnd_onResizeEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onResizeEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\sliderctrl.swg#L19" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onResizeEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ResizeEvent">ResizeEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_SliderCtrl_create"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">SliderCtrl.</span><span class="api-item-title-strong">create</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\sliderctrl.swg#L37" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Create the popup list, but do not display it. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">create</span>(parent: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>, name: <span class="STpe">string</span>, position: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span> = {}, id: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span> = <span class="SKwd">null</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SliderCtrl">SliderCtrl</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_SliderCtrl_setMinMaxF32"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">SliderCtrl.</span><span class="api-item-title-strong">setMinMaxF32</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\sliderctrl.swg#L96" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set a f32 range. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setMinMaxF32</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, min: <span class="STpe">f32</span>, max: <span class="STpe">f32</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_SliderCtrl_setMinMaxS64"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">SliderCtrl.</span><span class="api-item-title-strong">setMinMaxS64</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\sliderctrl.swg#L111" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set a f64 range. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setMinMaxS64</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, min: <span class="STpe">s64</span>, max: <span class="STpe">s64</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_SliderCtrl_setMinMaxU64"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">SliderCtrl.</span><span class="api-item-title-strong">setMinMaxU64</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\sliderctrl.swg#L103" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set a u64 range. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setMinMaxU64</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, min: <span class="STpe">u64</span>, max: <span class="STpe">u64</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_SliderCtrl_setValue"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">SliderCtrl.</span><span class="api-item-title-strong">setValue</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\sliderctrl.swg#L119" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the slider current value. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setValue</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, value: <span class="STpe">f32</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_SliderFlags"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">SliderFlags</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\slider.swg#L5" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">Zero</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Range</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">HilightBar</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Vertical</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Integer</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_SplitterCtrl"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">SplitterCtrl</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\splitterctrl.swg#L23" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> frameWnd</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_FrameWnd">FrameWnd</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">splitterFlags</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SplitterFlags">SplitterFlags</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">titleSize</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">correctMoving</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">hotIdx</span></td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">moving</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_SplitterCtrl_addPane"><span class="SCde"><span class="SFct">addPane</span>()</spa</a></td>
<td>Add a new pane. </td>
</tr>
<tr>
<td><a href="#Gui_SplitterCtrl_create"><span class="SCde"><span class="SFct">create</span>()</spa</a></td>
<td>Creates a new SplitterCtrl instance. </td>
</tr>
<tr>
<td><a href="#Gui_SplitterCtrl_isVertical"><span class="SCde"><span class="SFct">isVertical</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_SplitterCtrl_setPaneSize"><span class="SCde"><span class="SFct">setPaneSize</span>()</spa</a></td>
<td>Set the pane size (size of the first view). </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_SplitterCtrl_IWnd_onMouseEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onMouseEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\splitterctrl.swg#L37" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onMouseEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_MouseEvent">MouseEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_SplitterCtrl_IWnd_onPostPaintEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onPostPaintEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\splitterctrl.swg#L152" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onPostPaintEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_PaintEvent">PaintEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_SplitterCtrl_IWnd_onResizeEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onResizeEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\splitterctrl.swg#L102" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onResizeEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ResizeEvent">ResizeEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_SplitterCtrl_addPane"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">SplitterCtrl.</span><span class="api-item-title-strong">addPane</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\splitterctrl.swg#L403" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a new pane. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">addPane</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, pane: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>, paneSize: <span class="STpe">f32</span> = <span class="SNum">100</span>, title = <span class="SKwd">false</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SplitterItem">SplitterItem</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_SplitterCtrl_create"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">SplitterCtrl.</span><span class="api-item-title-strong">create</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\splitterctrl.swg#L384" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Creates a new SplitterCtrl instance. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">create</span>(parent: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>, position: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span> = {}, vertical = <span class="SKwd">false</span>, id: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span> = <span class="SKwd">null</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SplitterCtrl">SplitterCtrl</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_SplitterCtrl_isVertical"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">SplitterCtrl.</span><span class="api-item-title-strong">isVertical</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\splitterctrl.swg#L399" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">isVertical</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_SplitterCtrl_setPaneSize"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">SplitterCtrl.</span><span class="api-item-title-strong">setPaneSize</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\splitterctrl.swg#L439" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the pane size (size of the first view). </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setPaneSize</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, idx: <span class="STpe">u32</span>, newSize: <span class="STpe">f32</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_SplitterFlags"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">SplitterFlags</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\splitterctrl.swg#L5" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">Zero</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Vertical</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">HideBars</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_SplitterItem"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">SplitterItem</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\splitterctrl.swg#L12" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde">view</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">size</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">minSize</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">maxSize</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">label</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Label">Label</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">tryPreserveSize</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">aniBar</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_BlendColor">BlendColor</a></span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_StackLayoutCtrl"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">StackLayoutCtrl</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\stacklayoutctrl.swg#L17" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> wnd</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">layoutKind</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_StackLayoutKind">StackLayoutKind</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">spacing</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_StackLayoutCtrl_computeLayout"><span class="SCde"><span class="SFct">computeLayout</span>()</spa</a></td>
<td>Recompute layout of all childs. </td>
</tr>
<tr>
<td><a href="#Gui_StackLayoutCtrl_create"><span class="SCde"><span class="SFct">create</span>()</spa</a></td>
<td>Create a layout control. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_StackLayoutCtrl_IWnd_onResizeEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onResizeEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\stacklayoutctrl.swg#L27" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onResizeEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ResizeEvent">ResizeEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_StackLayoutCtrl_computeLayout"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">StackLayoutCtrl.</span><span class="api-item-title-strong">computeLayout</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\stacklayoutctrl.swg#L131" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Recompute layout of all childs. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">computeLayout</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_StackLayoutCtrl_create"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">StackLayoutCtrl.</span><span class="api-item-title-strong">create</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\stacklayoutctrl.swg#L123" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Create a layout control. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">create</span>(parent: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>, kind: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_StackLayoutKind">StackLayoutKind</a></span> = .<span class="SCst">Top</span>, position: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span> = {})-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_StackLayoutCtrl">StackLayoutCtrl</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_StackLayoutKind"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">StackLayoutKind</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\stacklayoutctrl.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">Top</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">TopAutoSize</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Bottom</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Left</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">LeftAutoSize</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Right</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">HorzCenter</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">HorzCenterVertCenter</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">VertCenter</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_StateEvent"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">StateEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\events.swg#L124" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> base</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Event">Event</a></span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Surface"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">Surface</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.swg#L11" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> native</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_NativeSurface">NativeSurface</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">app</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Application">Application</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">wnd</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SurfaceWnd">SurfaceWnd</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">painter</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Painter</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">rt</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">RenderTarget</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">userData</span></td>
<td class="code-type"><span class="SCde">*<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">rc</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">RenderingContext</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">minSize</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Point</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">maxSize</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Point</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">flags</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SurfaceFlags">SurfaceFlags</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">dirtyRect</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">colorDisabled</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">isDirty</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">isHidden</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">isDisabled</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">isMinimized</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> state</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SurfaceState">SurfaceState</a></span></span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_Surface_bringToTop"><span class="SCde"><span class="SFct">bringToTop</span>()</spa</a></td>
<td>Bring the surface at the top of the Z order. </td>
</tr>
<tr>
<td><a href="#Gui_Surface_center"><span class="SCde"><span class="SFct">center</span>()</spa</a></td>
<td>Center this surface relative to <span class="inline-code">from</span>. </td>
</tr>
<tr>
<td><a href="#Gui_Surface_constaintToScreen"><span class="SCde"><span class="SFct">constaintToScreen</span>()</spa</a></td>
<td>Constraint a window to be inside the closest monitor. </td>
</tr>
<tr>
<td><a href="#Gui_Surface_create"><span class="SCde"><span class="SFct">create</span>()</spa</a></td>
<td>Creates the surface. </td>
</tr>
<tr>
<td><a href="#Gui_Surface_destroy"><span class="SCde"><span class="SFct">destroy</span>()</spa</a></td>
<td>Destroy the surface. </td>
</tr>
<tr>
<td><a href="#Gui_Surface_disable"><span class="SCde"><span class="SFct">disable</span>()</spa</a></td>
<td>Enable the surface. </td>
</tr>
<tr>
<td><a href="#Gui_Surface_enable"><span class="SCde"><span class="SFct">enable</span>()</spa</a></td>
<td>Enable the surface. </td>
</tr>
<tr>
<td><a href="#Gui_Surface_getView"><span class="SCde"><span class="SFct">getView</span>()</spa</a></td>
<td>Returns inside view. </td>
</tr>
<tr>
<td><a href="#Gui_Surface_hide"><span class="SCde"><span class="SFct">hide</span>()</spa</a></td>
<td>Hide the surface. </td>
</tr>
<tr>
<td><a href="#Gui_Surface_invalidate"><span class="SCde"><span class="SFct">invalidate</span>()</spa</a></td>
<td>Invalidate the full surface to be painted. </td>
</tr>
<tr>
<td><a href="#Gui_Surface_invalidateRect"><span class="SCde"><span class="SFct">invalidateRect</span>()</spa</a></td>
<td>Invalidate one part of the surface to be painted. </td>
</tr>
<tr>
<td><a href="#Gui_Surface_paint"><span class="SCde"><span class="SFct">paint</span>()</spa</a></td>
<td>Main surface paint function. </td>
</tr>
<tr>
<td><a href="#Gui_Surface_screenToSurface"><span class="SCde"><span class="SFct">screenToSurface</span>()</spa</a></td>
<td>Transform a point in screen space relative to the surface. </td>
</tr>
<tr>
<td><a href="#Gui_Surface_setIcon"><span class="SCde"><span class="SFct">setIcon</span>()</spa</a></td>
<td>Set the surface icon. </td>
</tr>
<tr>
<td><a href="#Gui_Surface_setPosition"><span class="SCde"><span class="SFct">setPosition</span>()</spa</a></td>
<td>Set the surface position. </td>
</tr>
<tr>
<td><a href="#Gui_Surface_setSurfaceFlags"><span class="SCde"><span class="SFct">setSurfaceFlags</span>()</spa</a></td>
<td>Change flag surfaces. </td>
</tr>
<tr>
<td><a href="#Gui_Surface_setTitle"><span class="SCde"><span class="SFct">setTitle</span>()</spa</a></td>
<td>Set the surface title. </td>
</tr>
<tr>
<td><a href="#Gui_Surface_setView"><span class="SCde"><span class="SFct">setView</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Surface_show"><span class="SCde"><span class="SFct">show</span>()</spa</a></td>
<td>Show the surface. </td>
</tr>
<tr>
<td><a href="#Gui_Surface_showMaximized"><span class="SCde"><span class="SFct">showMaximized</span>()</spa</a></td>
<td>Show the surface as maximized. </td>
</tr>
<tr>
<td><a href="#Gui_Surface_showMinimized"><span class="SCde"><span class="SFct">showMinimized</span>()</spa</a></td>
<td>Show the surface as minimized. </td>
</tr>
<tr>
<td><a href="#Gui_Surface_showNormal"><span class="SCde"><span class="SFct">showNormal</span>()</spa</a></td>
<td>Show the surface in its default state. </td>
</tr>
<tr>
<td><a href="#Gui_Surface_surfaceToScreen"><span class="SCde"><span class="SFct">surfaceToScreen</span>()</spa</a></td>
<td>Transform a point relative to the surface in screen space. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Surface_bringToTop"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Surface.</span><span class="api-item-title-strong">bringToTop</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.win32.swg#L240" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Bring the surface at the top of the Z order. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">bringToTop</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Surface_center"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Surface.</span><span class="api-item-title-strong">center</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.swg#L169" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Center this surface relative to <span class="inline-code">from</span>. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">center</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, from: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Surface">Surface</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Surface_constaintToScreen"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Surface.</span><span class="api-item-title-strong">constaintToScreen</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.swg#L180" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Constraint a window to be inside the closest monitor. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">constaintToScreen</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Surface_create"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Surface.</span><span class="api-item-title-strong">create</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.swg#L97" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Creates the surface. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">create</span>(surface: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Surface">Surface</a></span>, x: <span class="STpe">s32</span>, y: <span class="STpe">s32</span>, width: <span class="STpe">s32</span>, height: <span class="STpe">s32</span>, flags: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SurfaceFlags">SurfaceFlags</a></span>, mainSurface: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Surface">Surface</a></span>) <span class="SKwd">throw</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Surface_destroy"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Surface.</span><span class="api-item-title-strong">destroy</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.swg#L121" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Destroy the surface. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">destroy</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Surface_disable"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Surface.</span><span class="api-item-title-strong">disable</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.win32.swg#L255" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Enable the surface. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">disable</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Surface_enable"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Surface.</span><span class="api-item-title-strong">enable</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.win32.swg#L246" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Enable the surface. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">enable</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Surface_getView"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Surface.</span><span class="api-item-title-strong">getView</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.swg#L128" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns inside view. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getView</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Surface_hide"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Surface.</span><span class="api-item-title-strong">hide</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.win32.swg#L285" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Hide the surface. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">hide</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Surface_invalidate"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Surface.</span><span class="api-item-title-strong">invalidate</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.swg#L145" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Invalidate the full surface to be painted. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">invalidate</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Surface_invalidateRect"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Surface.</span><span class="api-item-title-strong">invalidateRect</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.swg#L151" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Invalidate one part of the surface to be painted. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">invalidateRect</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, rect: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Surface_paint"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Surface.</span><span class="api-item-title-strong">paint</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.swg#L212" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Main surface paint function. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">paint</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Surface_screenToSurface"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Surface.</span><span class="api-item-title-strong">screenToSurface</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.win32.swg#L326" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Transform a point in screen space relative to the surface. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">screenToSurface</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, pt: *<span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Point</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Surface_setIcon"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Surface.</span><span class="api-item-title-strong">setIcon</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.swg#L131" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the surface icon. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setIcon</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, icon: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Icon">Icon</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Surface_setPosition"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Surface.</span><span class="api-item-title-strong">setPosition</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.win32.swg#L222" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the surface position. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setPosition</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, pos: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Surface_setSurfaceFlags"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Surface.</span><span class="api-item-title-strong">setSurfaceFlags</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.swg#L58" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Change flag surfaces. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setSurfaceFlags</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, add = <span class="SCst"><a href="#Gui_SurfaceFlags">SurfaceFlags</a></span>.<span class="SCst">Zero</span>, remove = <span class="SCst"><a href="#Gui_SurfaceFlags">SurfaceFlags</a></span>.<span class="SCst">Zero</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Surface_setTitle"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Surface.</span><span class="api-item-title-strong">setTitle</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.swg#L138" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the surface title. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setTitle</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, name: <span class="STpe">string</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Surface_setView"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Surface.</span><span class="api-item-title-strong">setView</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.swg#L290" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setView</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, view: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Surface_show"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Surface.</span><span class="api-item-title-strong">show</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.win32.swg#L264" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Show the surface. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">show</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Surface_showMaximized"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Surface.</span><span class="api-item-title-strong">showMaximized</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.win32.swg#L296" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Show the surface as maximized. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">showMaximized</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Surface_showMinimized"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Surface.</span><span class="api-item-title-strong">showMinimized</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.win32.swg#L317" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Show the surface as minimized. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">showMinimized</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Surface_showNormal"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Surface.</span><span class="api-item-title-strong">showNormal</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.win32.swg#L307" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Show the surface in its default state. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">showNormal</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Surface_surfaceToScreen"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Surface.</span><span class="api-item-title-strong">surfaceToScreen</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.win32.swg#L335" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Transform a point relative to the surface in screen space. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">surfaceToScreen</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, pt: *<span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Point</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_SurfaceFlags"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">SurfaceFlags</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.swg#L37" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">Zero</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">BigCaption</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">SmallCaption</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">MinimizeBtn</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">MaximizeBtn</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">CloseBtn</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Border</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Sizeable</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Secondary</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">TopMost</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">OverlappedWindow</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_SurfaceState"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">SurfaceState</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\surface.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde">position</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">positionNotMaximized</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">isMaximized</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_SurfaceWnd"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">SurfaceWnd</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\surfacewnd.swg#L13" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> base</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_FrameWnd">FrameWnd</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">icon</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Icon">Icon</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">minimizeBtn</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_IconButton">IconButton</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">maximizeBtn</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_IconButton">IconButton</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">closeBtn</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_IconButton">IconButton</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">movingSurface</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sizingBorder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SizingBorder">SizingBorder</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">cursorSet</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_SysCommandEvent"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">SysCommandEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\events.swg#L189" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> base</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Event">Event</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sysKind</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SysCommandEvent">SysCommandEvent</a></span>.<span class="SCst">Kind</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_SysUserEvent"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">SysUserEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\events.swg#L181" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> base</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Event">Event</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">userMsg</span></td>
<td class="code-type"><span class="STpe">u64</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">param0</span></td>
<td class="code-type"><span class="STpe">u64</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">param1</span></td>
<td class="code-type"><span class="STpe">u64</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Tab"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">Tab</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\tab.swg#L52" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> wnd</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">barForm</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_TabBarForm">TabBarForm</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">viewForm</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_TabViewForm">TabViewForm</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">barLayout</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_TabBarLayout">TabBarLayout</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sigSelChanged</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SigArray">SigArray</a></span>'(<span class="SKwd">closure</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Tab">Tab</a></span>))</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">buttonPopup</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_IconButton">IconButton</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">views</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">ArrayPtr</span>'(<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_TabItem">TabItem</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">popup</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_PopupListCtrl">PopupListCtrl</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">selectedIdx</span></td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">firstVisibleIdx</span></td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">hotIdx</span></td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">barHeight</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">mostRightPos</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_Tab_addItem"><span class="SCde"><span class="SFct">addItem</span>()</spa</a></td>
<td>Add a new view. </td>
</tr>
<tr>
<td><a href="#Gui_Tab_create"><span class="SCde"><span class="SFct">create</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Tab_select"><span class="SCde"><span class="SFct">select</span>()</spa</a></td>
<td>Select one tab at the given index. </td>
</tr>
<tr>
<td><a href="#Gui_Tab_setForm"><span class="SCde"><span class="SFct">setForm</span>()</spa</a></td>
<td>Set the tab form. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Tab_IWnd_onMouseEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onMouseEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\tab.swg#L243" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onMouseEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_MouseEvent">MouseEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Tab_IWnd_onPaintEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onPaintEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\tab.swg#L165" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onPaintEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_PaintEvent">PaintEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Tab_IWnd_onResizeEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onResizeEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\tab.swg#L225" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onResizeEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ResizeEvent">ResizeEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Tab_IWnd_paintItem"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">paintItem</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\tab.swg#L78" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">paintItem</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, painter: *<span class="SCst">Pixel</span>.<span class="SCst">Painter</span>, v: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_TabItem">TabItem</a></span>, idx: <span class="STpe">u32</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Tab_addItem"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Tab.</span><span class="api-item-title-strong">addItem</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\tab.swg#L473" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a new view. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">addItem</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, tabName: <span class="STpe">string</span>, icon: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Icon">Icon</a></span>, view: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span> = <span class="SKwd">null</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Tab_create"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Tab.</span><span class="api-item-title-strong">create</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\tab.swg#L435" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">create</span>(parent: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>, position: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span> = {}, id: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span> = <span class="SKwd">null</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Tab">Tab</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Tab_select"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Tab.</span><span class="api-item-title-strong">select</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\tab.swg#L494" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Select one tab at the given index. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">select</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, idx: <span class="STpe">u32</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Tab_setForm"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Tab.</span><span class="api-item-title-strong">setForm</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\tab.swg#L445" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the tab form. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setForm</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, tabForm: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_TabForm">TabForm</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_TabBarForm"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">TabBarForm</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\tab.swg#L11" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">Transparent</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Flat</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">FlatRound</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Round</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_TabBarLayout"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">TabBarLayout</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\tab.swg#L35" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">Divide</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Fixed</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Auto</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_TabForm"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">TabForm</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\tab.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">Flat</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Round</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Button</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_TabItem"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">TabItem</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\tab.swg#L42" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde">pos</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">width</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">view</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">aniImg</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_BlendColor">BlendColor</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">aniText</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_BlendColor">BlendColor</a></span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_TabItemForm"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">TabItemForm</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\tab.swg#L20" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">Transparent</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Flat</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Round</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Button</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_TabViewForm"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">TabViewForm</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\tab.swg#L28" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">None</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Flat</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Round</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Theme"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">Theme</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\theme.swg#L52" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde">intialized</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">res</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeResources">ThemeResources</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">rects</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRects">ThemeImageRects</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">metrics</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeMetrics">ThemeMetrics</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">colors</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeColors">ThemeColors</a></span></span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_Theme_createDefaultFont"><span class="SCde"><span class="SFct">createDefaultFont</span>()</spa</a></td>
<td>Create a default font with the given size. </td>
</tr>
<tr>
<td><a href="#Gui_Theme_createDefaultFontFS"><span class="SCde"><span class="SFct">createDefaultFontFS</span>()</spa</a></td>
<td>Create a default font (fixed size) with the given size. </td>
</tr>
<tr>
<td><a href="#Gui_Theme_drawIconText"><span class="SCde"><span class="SFct">drawIconText</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Theme_drawSubRect"><span class="SCde"><span class="SFct">drawSubRect</span>(<span class="SKwd">self</span>, *<span class="SCst">Painter</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Rectangle</span>, *<span class="SCst">ThemeImageRect</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Color</span>, <span class="STpe">bool</span>, <span class="SCst">InterpolationMode</span>)</span></a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Theme_drawSubRect"><span class="SCde"><span class="SFct">drawSubRect</span>(*<span class="SCst">Painter</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Texture</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Rectangle</span>, *<span class="SCst">ThemeImageRect</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Color</span>, <span class="STpe">bool</span>, <span class="SCst">InterpolationMode</span>)</span></a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Theme_getIcon24"><span class="SCde"><span class="SFct">getIcon24</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Theme_getIcon64"><span class="SCde"><span class="SFct">getIcon64</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Theme_setTextColors"><span class="SCde"><span class="SFct">setTextColors</span>()</spa</a></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Theme_createDefaultFont"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Theme.</span><span class="api-item-title-strong">createDefaultFont</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\theme.swg#L111" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Create a default font with the given size. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">createDefaultFont</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, size: <span class="STpe">u32</span>, style = <span class="SCst">FontFamilyStyle</span>.<span class="SCst">Regular</span>)-&gt;*<span class="SCst">Pixel</span>.<span class="SCst">Font</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Theme_createDefaultFontFS"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Theme.</span><span class="api-item-title-strong">createDefaultFontFS</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\theme.swg#L126" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Create a default font (fixed size) with the given size. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">createDefaultFontFS</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, size: <span class="STpe">u32</span>, style = <span class="SCst">FontFamilyStyle</span>.<span class="SCst">Regular</span>)-&gt;*<span class="SCst">Pixel</span>.<span class="SCst">Font</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Theme_drawIconText"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Theme.</span><span class="api-item-title-strong">drawIconText</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\themepaint.swg#L93" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">drawIconText</span>(painter: *<span class="SCst">Pixel</span>.<span class="SCst">Painter</span>, position: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span>, icon: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Icon">Icon</a></span>, name: *<span class="SCst">Pixel</span>.<span class="SCst">RichString</span>, stringFormat: <span class="SCst">Pixel</span>.<span class="SCst">RichStringFormat</span>, iconMargin = <span class="SNum">4</span>'<span class="STpe">f32</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Theme_drawSubRect"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Theme.</span><span class="api-item-title-strong">drawSubRect</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\themepaint.swg#L6" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">drawSubRect</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, painter: *<span class="SCst">Pixel</span>.<span class="SCst">Painter</span>, dstRect: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span>, subRect: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span>, color: <span class="SCst">Pixel</span>.<span class="SCst">Color</span>, center = <span class="SKwd">true</span>, interpol = <span class="SCst">InterpolationMode</span>.<span class="SCst">Linear</span>)
<span class="SKwd">func</span> <span class="SFct">drawSubRect</span>(painter: *<span class="SCst">Pixel</span>.<span class="SCst">Painter</span>, texture: <span class="SCst">Pixel</span>.<span class="SCst">Texture</span>, dstRect: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span>, subRect: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span>, color: <span class="SCst">Pixel</span>.<span class="SCst">Color</span>, center = <span class="SKwd">true</span>, interpol = <span class="SCst">InterpolationMode</span>.<span class="SCst">Linear</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Theme_getIcon24"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Theme.</span><span class="api-item-title-strong">getIcon24</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\themeicons.swg#L42" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getIcon24</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, idx: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeIcons24">ThemeIcons24</a></span>, size = <span class="SNum">24</span>'<span class="STpe">f32</span>)-&gt;<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Icon">Icon</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Theme_getIcon64"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Theme.</span><span class="api-item-title-strong">getIcon64</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\themeicons.swg#L52" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getIcon64</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, idx: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeIcons64">ThemeIcons64</a></span>, size = <span class="SNum">64</span>'<span class="STpe">f32</span>)-&gt;<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Icon">Icon</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Theme_setTextColors"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Theme.</span><span class="api-item-title-strong">setTextColors</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\themepaint.swg#L121" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setTextColors</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, fmt: *<span class="SCst">Pixel</span>.<span class="SCst">RichStringFormat</span>, mainColor: <span class="SCst">Pixel</span>.<span class="SCst">Color</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ThemeColors"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">ThemeColors</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\themecolors.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">Transparent</span></td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Blue</span></td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">LightBlue</span></td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Disabled</span></td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">LighterSmall</span></td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">LighterMedium</span></td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">LighterBig</span></td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">LighterVeryBig</span></td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">White</span></td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Gray1</span></td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Gray2</span></td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Gray3</span></td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Gray4</span></td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Gray5</span></td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Gray6</span></td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Gray7</span></td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Gray8</span></td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">richColor1</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">richColor2</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">richColor3</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">surfaceDisabled</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">hilight</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">hilightLight</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">hilightDark</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">disabled</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">transparent</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">lighterSmall</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">lighterMedium</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">lighterBig</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">lighterVeryBig</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">wnd_Bk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">wnd_Caption</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">wnd_CaptionNotActived</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">wnd_CaptionText</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">wnd_CaptionNotActivatedText</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">wnd_BtnCloseBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">wnd_BtnCloseHotBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">wnd_Shadow</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">view_Bk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">dlg_Bk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">dlg_BtnBarBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnPush_StrongText</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnPush_StrongBorder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnPush_StrongBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnPush_Text</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnPush_Border</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnPush_Bk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnPush_PressedText</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnPush_PressedBorder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnPush_PressedBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnPush_HotText</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnPush_HotBorder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnPush_HotBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnPush_DisabledText</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnPush_DisabledBorder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnPush_DisabledBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnPushFlat_DisabledText</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnPushFlat_DisabledBorder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnPushFlat_DisabledBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnPushFlat_Text</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnPushFlat_Border</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnPushFlat_Bk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnPushFlat_PressedText</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnPushFlat_PressedBorder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnPushFlat_PressedBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnPushFlat_HotText</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnPushFlat_HotBorder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnPushFlat_HotBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnPushDefault_DisabledText</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnPushDefault_DisabledBorder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnPushDefault_DisabledBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnPushDefault_Text</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnPushDefault_Border</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnPushDefault_Bk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnPushDefault_PressedText</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnPushDefault_PressedBorder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnPushDefault_PressedBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnPushDefault_HotText</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnPushDefault_HotBorder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnPushDefault_HotBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnIcon_Icon</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnIcon_Text</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnIcon_Bk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnIcon_DisabledIcon</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnIcon_DisabledText</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnIcon_DisabledBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnIcon_PressedIcon</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnIcon_PressedText</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnIcon_PressedBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnIcon_HotIcon</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnIcon_HotText</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnIcon_HotBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnIcon_CheckedDisabledIcon</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnIcon_CheckedDisabledText</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnIcon_CheckedPressedIcon</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnIcon_CheckedPressedText</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnIcon_CheckedHotIcon</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnIcon_CheckedHotText</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnIcon_CheckedIcon</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnIcon_CheckedText</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnIcon_CheckedDisabledBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnIcon_CheckedPressedBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnIcon_CheckedHotBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnIcon_CheckedBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnIcon_CheckedMark</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnIcon_CheckedPressedMark</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnIcon_CheckedHotMark</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnIcon_CheckedDisabledMark</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">label_Text</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">label_TextLight</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">label_Text1</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">label_Text2</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">label_Text3</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">label_DisabledText</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">label_Icon</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">label_DisabledIcon</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnCheck_Text</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnCheck_Border</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnCheck_Mark</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnCheck_Bk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnCheck_PressedText</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnCheck_PressedBorder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnCheck_PressedMark</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnCheck_PressedBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnCheck_HotText</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnCheck_HotBorder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnCheck_HotMark</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnCheck_HotBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnCheck_DisabledText</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnCheck_DisabledBorder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnCheck_DisabledMark</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnCheck_DisabledBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnCheck_CheckedText</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnCheck_CheckedBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnCheck_CheckedBorder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnCheck_CheckedMark</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">toolTip_Bk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">toolTip_Border</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">toolTip_Text</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">edit_Border</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">edit_FocusBorder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">edit_HotBorder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">edit_DisabledBorder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">edit_Bk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">edit_DisabledBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">edit_ErrorBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">edit_HotBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">edit_FocusBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">edit_Note</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">edit_Text</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">edit_DisabledText</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">edit_SelectedText</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">edit_SelectedBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">edit_ErrorBorder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">richEdit_Bk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">richEdit_TextSpecial</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">richEdit_Text</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">richEdit_SelText</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">richEdit_SelTextNoFocus</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">richEdit_SelBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">richEdit_SelBkNoFocus</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">richEdit_Caret</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">richEdit_CurLineBorder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnToggle_DisabledBorder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnToggle_DisabledMark</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnToggle_DisabledBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnToggle_OffBorder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnToggle_OffMark</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnToggle_OffBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnToggle_OffHotBorder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnToggle_OffHotMark</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnToggle_OffHotBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnToggle_OffPressedBorder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnToggle_OffPressedMark</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnToggle_OffPressedBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnToggle_OnBorder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnToggle_OnMark</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnToggle_OnBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnToggle_OnHotBorder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnToggle_OnHotMark</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnToggle_OnHotBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnToggle_OnPressedBorder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnToggle_OnPressedMark</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnToggle_OnPressedBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">slider_BkLeft</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">slider_BkMiddle</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">slider_BkRight</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">slider_HilightBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">slider_Mark</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">slider_HotMark</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">slider_PressedMark</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">slider_DisabledMark</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">slider_DisabledBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">slider_StepBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">slider_HilightStepBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">slider_HotStepBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">tabItem_DisabledText</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">tabItem_SelectedText</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">tabItem_UnSelectedText</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">tabItem_FlatSelected</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">tabItem_FlatHot</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">tabItem_RoundSelected</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">tabItem_RoundHot</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">tabItem_ButtonSelected</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">tabItem_ButtonHot</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">tabBar_FlatBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">tabBar_RoundBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">tabView_FlatBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">tabView_RoundBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">menuPopup_Bk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">menuPopup_Border</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">menuPopup_Shadow</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">menuPopup_Separator</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">menuBar_Bk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">menuBar_Border</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">menuBar_SelectedBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">popupItem_Text</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">popupItem_RightText</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">popupItem_DisabledText</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">popupItem_HotText</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">popupItem_HotRightText</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">popupItem_HotBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">scrollBar_Bk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">scrollBar_HotBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">scrollBar_DisabledBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">scrollBar_Box</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">scrollBar_HotBox</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">scrollBar_PressedBox</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">comboBox_Border</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">comboBox_HotBorder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">comboBox_DisabledBorder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">comboBox_SelectedBorder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">comboBox_Bk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">comboBox_HotBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">comboBox_DisabledBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">comboBox_SelectedBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">comboBox_Text</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">comboBox_HotText</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">comboBox_DisabledText</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">comboBox_SelectedText</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">comboBoxItem_Text</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">comboBoxItem_HotBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">comboBoxItem_HotText</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">comboBoxItem_CheckMark</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">comboBoxItem_HotCheckMark</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">popupList_Separator</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">popupList_HotBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">popupList_Text</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">popupList_HotText</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">popupList_CheckMark</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">popupList_HotCheckMark</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">frameWnd_SmallShadow</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">frameWnd_Border</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">frameWnd_PopupBorder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">frameWnd_FocusBorder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">frameWnd_DisabledBorder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">frameWnd_Bk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">frameWnd_DisabledBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">frameWnd_FocusBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">listItem_FocusSelectedBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">listItem_SelectedBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">listItem_HotBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">listItem_DisabledText</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">listItem_SelectedText</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">listItem_Text</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">listItem_Separator</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">listItem_AlternateBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">listItem_Line</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">headerItem_DisabledText</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">headerItem_HotText</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">headerItem_HotBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">headerItem_ClickedBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">headerItem_Text</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">headerItem_Separator</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">header_Bk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">splitView_TitleBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">splitView_Bar</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">splitView_BarHot</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">progressBar_DisabledBorder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">progressBar_DisabledBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">progressBar_DisabledMark</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">progressBar_Border</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">progressBar_Bk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">progressBar_Mark</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">palPicker_Border</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">palPicker_HotBorder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">colorPicker_Border</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">colorPicker_BackAlpha</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">colorPicker_Mark</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">imageRect_Fg</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">embInfo_CriticalBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">embInfo_InformationBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">propList_focusBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">propList_focusBorder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Color</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ThemeIcons24"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">ThemeIcons24</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\themeicons.swg#L3" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">WndMinimize</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">WndMaximize</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">WndClose</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Search</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">RightPopupArrow</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">BottomPopupArrow</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">SortArrowDown</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">SortArrowUp</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">SortArrowRight</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Picker</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Play</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Pause</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Stop</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">SendToBack</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">SendBackward</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">BringForward</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">BringToFront</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Plus</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">More</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Undo</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Redo</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Copy</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Paste</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Cut</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ThemeIcons64"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">ThemeIcons64</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\themeicons.swg#L31" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">Question</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Warning</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Critical</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Information</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">HourGlass</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ThemeImageRect"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">ThemeImageRect</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\themerects.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde">rect</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">corner</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">keepBordersRatio</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ThemeImageRects"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">ThemeImageRects</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\themerects.swg#L11" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde">surfaceWnd_Shadow</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">wnd_Border</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">wnd_Caption</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">wnd_CaptionSmall</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">dlg_BtnBar</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnPush_Normal</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnPush_NormalBorder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnPush_Hot</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnPush_HotBorder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnPush_Pressed</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnPush_PressedBorder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnPush_Disabled</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnPush_DisabledBorder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnIcon_SquareBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnIcon_RoundBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnIcon_RoundSquareBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnIcon_CheckedBottom</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnIcon_CheckedTop</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnIcon_CheckedLeft</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnIcon_CheckedRight</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnIcon_CheckedFull</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnIcon_ArrowDown</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnIcon_ArrowLeft</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnIcon_ArrowUp</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnIcon_ArrowRight</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnCheck_Normal</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnCheck_NormalBorder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnCheck_Undefined</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnCheck_UndefinedBorder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnCheck_UndefinedMark</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnCheck_Checked</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnCheck_CheckedBorder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnCheck_CheckedMark</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnRadio_NormalBorder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnRadio_Normal</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnRadio_CheckedBorder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnRadio_Checked</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnRadio_CheckedMark</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">edit_SquareBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">edit_SquareBorder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">edit_RoundBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">edit_RoundBorder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">edit_FlatBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">edit_FlatBorder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnToggle_Bk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnToggle_Border</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnToggle_Mark</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">slider_BkLeft</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">slider_BkMiddle</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">slider_BkRight</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">slider_BkLeftVert</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">slider_BkMiddleVert</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">slider_BkRightVert</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">slider_Mark</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">slider_Step</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">tabItem_FlatSelected</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">tabItem_RoundSelected</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">tabItem_RoundHot</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">tabItem_ButtonSelected</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">tabItem_ButtonHot</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">tabBar_FlatBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">tabBar_FlatRoundBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">tabBar_RoundBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">tabView_FlatBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">tabView_RoundBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">menuPopup_Shadow</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">menuPopup_Bk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">menuPopup_Border</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">menuPopup_HotBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">menuBar_Bk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">menuBar_Border</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">menuBar_SelectedBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">menuItem_CheckedMark</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">scrollBar_BkV</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">scrollBar_BkH</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">scrollBar_BoxV</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">scrollBar_BoxH</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">scrollBar_Corner</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">frameWnd_SmallShadow</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">frameWnd_SquareBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">frameWnd_SquareBorder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">frameWnd_RoundBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">frameWnd_RoundBorder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">frameWnd_AnchorTop</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">frameWnd_AnchorTopBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">frameWnd_AnchorBottom</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">frameWnd_AnchorBottomBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">frameWnd_AnchorRight</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">frameWnd_AnchorRightBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">frameWnd_AnchorLeft</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">frameWnd_AnchorLeftBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">header_FlatBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">header_RoundBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">selectionBox_Bk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">splitView_HorzBarBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">splitView_HorzBarHotBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">splitView_VertBarBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">splitView_VertBarHotBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">progressBar_Bk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">progressBar_Border</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">progressBar_Mark</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">colorPicker_RoundPick</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">colorPicker_SquarePick</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">colorPicker_Bk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">colorPicker_Border</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRect">ThemeImageRect</a></span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ThemeMetrics"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">ThemeMetrics</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\thememetrics.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde">defaultFontSize</span></td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">defaultFixedFontSize</span></td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">surfaceWnd_ShadowSize</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">wnd_BorderSize</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">wnd_BigCaptionCY</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">wnd_SmallCaptionCY</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">wnd_CaptionMarginTitle</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">tabBar_FlatHeight</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">tabBar_RoundHeight</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">tabBar_ButtonHeight</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnPush_Padding</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnPush_Width</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnPush_Height</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">iconText_Margin</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnCheck_Size</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnCheck_Padding</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">dialog_Padding</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">dialog_BtnPadding</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">editBox_SquarePadding</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Vector4</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">editBox_RoundPadding</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Vector4</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">editBox_FlatPadding</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Vector4</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">editBox_Width</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">editBox_Height</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnToggle_Width</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnToggle_Height</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">slider_Width</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">slider_Height</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">slider_BoxSize</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">slider_StepSize</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">menuPopup_Padding</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">menuPopup_SpaceLines</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">menuPopup_ShadowSize</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">menuPopup_IconTextMargin</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">menuPopup_SeparatorHeight</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">menuBar_Height</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">menuBar_SpaceBetween</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">iconBar_ButtonsPadding</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">scrollBar_NormalSize</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">scrollBar_SmallSize</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">scrollBar_BoxPadding</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">scrollBar_BoxMinSize</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">comboBox_Width</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">comboBox_Height</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">comboBox_RightMargin</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">comboBox_Padding</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">comboBox_IconTextMargin</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">popupList_Padding</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">popupList_IconTextMargin</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">popupList_SeparatorPadding</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnIcon_WidthArrow</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnIcon_HeightArrow</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnIcon_Padding</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnIcon_MarginArrow</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnIcon_SpacingIcon</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnIcon_SpacingArrowH</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnIcon_SpacingArrowV</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnIcon_SpacingTextH</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">btnIcon_SpacingTextV</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">frameWnd_SquarePadding</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">frameWnd_RoundPadding</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">frameWnd_FlatPadding</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">frameWnd_SmallShadowSize</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Vector4</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">frameWnd_AnchorWidth</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">frameWnd_AnchorHeight</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">frameWnd_AnchorOverlap</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">frameWnd_AnchorRoundBorderMargin</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">frameWnd_AnchorSquareBorderMargin</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">list_Padding</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">list_ExpandSize</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">header_Height</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">header_Padding</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Vector4</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">header_Separator</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">splitView_BarSize</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">progressBar_Height</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">progressBar_ClipOffset</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">progressBar_SpeedInf</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">colorPicker_RoundPick</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">colorPicker_SquarePick</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">label_Height</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">richEdit_Padding</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">toolTip_Padding</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ThemeResources"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">ThemeResources</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\theme.swg#L34" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde">imgWidgets</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">Texture</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">icons24</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ImageList">ImageList</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">icons64</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ImageList">ImageList</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">spin</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ImageList">ImageList</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">defaultTypeFaceR</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Pixel</span>.<span class="SCst">TypeFace</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">defaultTypeFaceB</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Pixel</span>.<span class="SCst">TypeFace</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">defaultTypeFaceI</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Pixel</span>.<span class="SCst">TypeFace</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">defaultTypeFaceBI</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Pixel</span>.<span class="SCst">TypeFace</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">defaultFsTypeFaceR</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Pixel</span>.<span class="SCst">TypeFace</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">defaultFsTypeFaceB</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Pixel</span>.<span class="SCst">TypeFace</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">defaultFsTypeFaceI</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Pixel</span>.<span class="SCst">TypeFace</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">defaultFsTypeFaceBI</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Pixel</span>.<span class="SCst">TypeFace</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">fontDefault</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">FontFamily</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">fontDefaultFs</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">FontFamily</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ThemeStyle"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">ThemeStyle</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\themestyle.swg#L10" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde">refr</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeStyleRef">ThemeStyleRef</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">dirty</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">styleSheetMetrics</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">String</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">styleSheetColors</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">String</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">font</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">FontFamily</span></span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_ThemeStyle_addStyleSheetColors"><span class="SCde"><span class="SFct">addStyleSheetColors</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_ThemeStyle_addStyleSheetMetrics"><span class="SCde"><span class="SFct">addStyleSheetMetrics</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_ThemeStyle_clearFont"><span class="SCde"><span class="SFct">clearFont</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_ThemeStyle_clearStyleSheetColors"><span class="SCde"><span class="SFct">clearStyleSheetColors</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_ThemeStyle_create"><span class="SCde"><span class="SFct">create</span>()</spa</a></td>
<td>Create a specific style based on the parent. </td>
</tr>
<tr>
<td><a href="#Gui_ThemeStyle_getFont"><span class="SCde"><span class="SFct">getFont</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_ThemeStyle_setFont"><span class="SCde"><span class="SFct">setFont</span>(<span class="SKwd">self</span>, *<span class="SCst">Font</span>, <span class="SCst">FontFamilyStyle</span>)</span></a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_ThemeStyle_setFont"><span class="SCde"><span class="SFct">setFont</span>(<span class="SKwd">self</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">FontFamily</span>)</span></a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_ThemeStyle_setStyleSheetColors"><span class="SCde"><span class="SFct">setStyleSheetColors</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_ThemeStyle_setStyleSheetMetrics"><span class="SCde"><span class="SFct">setStyleSheetMetrics</span>()</spa</a></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ThemeStyle_addStyleSheetColors"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ThemeStyle.</span><span class="api-item-title-strong">addStyleSheetColors</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\themestyle.swg#L128" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">addStyleSheetColors</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, str: <span class="STpe">string</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ThemeStyle_addStyleSheetMetrics"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ThemeStyle.</span><span class="api-item-title-strong">addStyleSheetMetrics</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\themestyle.swg#L141" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">addStyleSheetMetrics</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, str: <span class="STpe">string</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ThemeStyle_clearFont"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ThemeStyle.</span><span class="api-item-title-strong">clearFont</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\themestyle.swg#L171" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">clearFont</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ThemeStyle_clearStyleSheetColors"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ThemeStyle.</span><span class="api-item-title-strong">clearStyleSheetColors</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\themestyle.swg#L116" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">clearStyleSheetColors</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ThemeStyle_create"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ThemeStyle.</span><span class="api-item-title-strong">create</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\themestyle.swg#L48" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Create a specific style based on the parent. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">create</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, parent: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeStyle">ThemeStyle</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ThemeStyle_getFont"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ThemeStyle.</span><span class="api-item-title-strong">getFont</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\themestyle.swg#L148" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getFont</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst">FontFamily</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ThemeStyle_setFont"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ThemeStyle.</span><span class="api-item-title-strong">setFont</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\themestyle.swg#L165" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setFont</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, fnt: *<span class="SCst">Pixel</span>.<span class="SCst">Font</span>, style: <span class="SCst">Pixel</span>.<span class="SCst">FontFamilyStyle</span>)
<span class="SKwd">func</span> <span class="SFct">setFont</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, fnt: <span class="SCst">Pixel</span>.<span class="SCst">FontFamily</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ThemeStyle_setStyleSheetColors"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ThemeStyle.</span><span class="api-item-title-strong">setStyleSheetColors</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\themestyle.swg#L122" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setStyleSheetColors</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, str: <span class="STpe">string</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ThemeStyle_setStyleSheetMetrics"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ThemeStyle.</span><span class="api-item-title-strong">setStyleSheetMetrics</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\themestyle.swg#L135" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setStyleSheetMetrics</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, str: <span class="STpe">string</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ThemeStyleRef"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">ThemeStyleRef</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\paint\themestyle.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde">theme</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Theme">Theme</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">count</span></td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Timer"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">Timer</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\timer.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde">timeMs</span></td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">target</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">stopwatch</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Time</span>.<span class="SCst">Stopwatch</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">toDelete</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_Timer_restart"><span class="SCde"><span class="SFct">restart</span>()</spa</a></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Timer_restart"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Timer.</span><span class="api-item-title-strong">restart</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\timer.swg#L15" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">restart</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_TimerEvent"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">TimerEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\events.swg#L107" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> base</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Event">Event</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">timer</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Timer">Timer</a></span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ToggleButton"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">ToggleButton</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\togglebutton.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> button</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Button">Button</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">isToggled</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">sigChanged</span></td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ToggleButton">ToggleButton</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">aniBk</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_BlendColor">BlendColor</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">aniBorder</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_BlendColor">BlendColor</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">aniMark</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_BlendColor">BlendColor</a></span></span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_ToggleButton_create"><span class="SCde"><span class="SFct">create</span>()</spa</a></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ToggleButton_IWnd_onMouseEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onMouseEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\togglebutton.swg#L74" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onMouseEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_MouseEvent">MouseEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ToggleButton_IWnd_onPaintEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onPaintEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\togglebutton.swg#L18" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onPaintEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_PaintEvent">PaintEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ToggleButton_create"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ToggleButton.</span><span class="api-item-title-strong">create</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\widgets\togglebutton.swg#L89" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">create</span>(parent: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>, position: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span> = {}, id: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span> = <span class="SKwd">null</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ToggleButton">ToggleButton</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ToolTip_hide"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ToolTip.</span><span class="api-item-title-strong">hide</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\tooltip.swg#L40" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Hide the current tooltip, if visible. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">hide</span>()</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_ToolTip_show"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ToolTip.</span><span class="api-item-title-strong">show</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\tooltip.swg#L72" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Show a tooltip. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">show</span>(owner: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>, surfacePos: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Point</span>, str: <span class="STpe">string</span>, delay: <span class="SCst">Core</span>.<span class="SCst">Time</span>.<span class="SCst">Duration</span> = <span class="SNum">500</span>'ms)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">Wnd</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L89" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde">itf</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_IWnd">IWnd</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">type</span></td>
<td class="code-type"><span class="SCde"><span class="SKwd">const</span> *<span class="SCst">Swag</span>.<span class="SCst">TypeInfo</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">surface</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Surface">Surface</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">style</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeStyle">ThemeStyle</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">wndFlags</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndFlags">WndFlags</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">userData</span></td>
<td class="code-type"><span class="SCde">*<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">position</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">minSize</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Point</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">maxSize</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Point</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">scrollPos</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Point</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">invalidatePadding</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Vector4</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">parent</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">owner</span></td>
<td class="code-type"><span class="SCde">*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">childs</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">name</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst">RichString</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">id</span></td>
<td class="code-type"><span class="STpe">string</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">cursor</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Cursor">Cursor</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">toolTip</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">String</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">keyShortcuts</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'(<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_KeyShortcut">KeyShortcut</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">actions</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'(<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_IActionUI">IActionUI</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">margin</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Vector4</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">padding</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Vector4</span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">dockStyle</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_DockStyle">DockStyle</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">anchorStyle</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_AnchorStyle">AnchorStyle</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">focusStrategy</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_FocusStategy">FocusStategy</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">backgroundStyle</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_BackgroundStyle">BackgroundStyle</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">isAllocated</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">isPendingDestroy</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">createEventDone</span></td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">hookOnEvent</span></td>
<td class="code-type"><span class="SCde"><span class="SKwd">closure</span>(*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>, *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Event">Event</a></span>)-&gt;<span class="STpe">bool</span></span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_Wnd_applyLayout"><span class="SCde"><span class="SFct">applyLayout</span>(<span class="SKwd">self</span>)</span></a></td>
<td>Apply the current layout (childs constraints). </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_applyLayout"><span class="SCde"><span class="SFct">applyLayout</span>(<span class="SKwd">self</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Point</span>)</span></a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_bringToFront"><span class="SCde"><span class="SFct">bringToFront</span>()</spa</a></td>
<td>Force the window to be the first painted (before all siblings). </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_bringToTop"><span class="SCde"><span class="SFct">bringToTop</span>()</spa</a></td>
<td>Force the window to be the last painted (on top of siblings). </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_captureMouse"><span class="SCde"><span class="SFct">captureMouse</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_computeStyle"><span class="SCde"><span class="SFct">computeStyle</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_create"><span class="SCde"><span class="SFct">create</span>(*<span class="SCst">Wnd</span>, <span class="SCst">WndId</span>, <span class="SCst">HookEvent</span>)</span></a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_create"><span class="SCde"><span class="SFct">create</span>(*<span class="SCst">Wnd</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Rectangle</span>, <span class="SCst">WndId</span>, <span class="SCst">HookEvent</span>)</span></a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_create"><span class="SCde"><span class="SFct">create</span>(*<span class="SCst">Wnd</span>, <span class="STpe">string</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Rectangle</span>, <span class="SCst">WndId</span>, <span class="SCst">HookEvent</span>)</span></a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_destroy"><span class="SCde"><span class="SFct">destroy</span>()</spa</a></td>
<td>Destroy the window. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_destroyNow"><span class="SCde"><span class="SFct">destroyNow</span>()</spa</a></td>
<td>Destroy the window. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_disable"><span class="SCde"><span class="SFct">disable</span>()</spa</a></td>
<td>Disable the window. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_enable"><span class="SCde"><span class="SFct">enable</span>()</spa</a></td>
<td>Enable/Disable the window. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_fitPosInParent"><span class="SCde"><span class="SFct">fitPosInParent</span>()</spa</a></td>
<td>Be sure rect is fully inside the parent rectangle (if possible). </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_getApp"><span class="SCde"><span class="SFct">getApp</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_getChildById"><span class="SCde"><span class="SFct">getChildById</span>()</spa</a></td>
<td>Retrieve the child with the given id. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_getClientRect"><span class="SCde"><span class="SFct">getClientRect</span>()</spa</a></td>
<td>Returns the client area. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_getClientRectPadding"><span class="SCde"><span class="SFct">getClientRectPadding</span>()</spa</a></td>
<td>Returns the client area, with <span class="inline-code">padding</span> applied. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_getFocus"><span class="SCde"><span class="SFct">getFocus</span>()</spa</a></td>
<td>Get the window with the keyboard focus. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_getFont"><span class="SCde"><span class="SFct">getFont</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_getKeyShortcut"><span class="SCde"><span class="SFct">getKeyShortcut</span>()</spa</a></td>
<td>Get the id associated with a shortcut. null if none. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_getKeyShortcutNameFor"><span class="SCde"><span class="SFct">getKeyShortcutNameFor</span>()</spa</a></td>
<td>Get the name associated with a given id shortcut. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_getMouseCapture"><span class="SCde"><span class="SFct">getMouseCapture</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_getOwner"><span class="SCde"><span class="SFct">getOwner</span>()</spa</a></td>
<td>Get the owner of the window. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_getParentById"><span class="SCde"><span class="SFct">getParentById</span>()</spa</a></td>
<td>Retrieve the parent with the given id. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_getRectIn"><span class="SCde"><span class="SFct">getRectIn</span>()</spa</a></td>
<td>Get the wnd position relative to a given parent. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_getSurfaceRect"><span class="SCde"><span class="SFct">getSurfaceRect</span>()</spa</a></td>
<td>Get the wnd position in the surface. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_getTheme"><span class="SCde"><span class="SFct">getTheme</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_getThemeColors"><span class="SCde"><span class="SFct">getThemeColors</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_getThemeMetrics"><span class="SCde"><span class="SFct">getThemeMetrics</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_getThemeRects"><span class="SCde"><span class="SFct">getThemeRects</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_getTiming"><span class="SCde"><span class="SFct">getTiming</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_getTopView"><span class="SCde"><span class="SFct">getTopView</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_getTopWnd"><span class="SCde"><span class="SFct">getTopWnd</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_getWndAt"><span class="SCde"><span class="SFct">getWndAt</span>()</spa</a></td>
<td>Get the child window at the given coordinate  Coordinate is expressed in the parent system. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_hasFocus"><span class="SCde"><span class="SFct">hasFocus</span>()</spa</a></td>
<td>Returns true if the window has the keyboard focus. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_hide"><span class="SCde"><span class="SFct">hide</span>()</spa</a></td>
<td>Hide the window. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_invalidate"><span class="SCde"><span class="SFct">invalidate</span>()</spa</a></td>
<td>Force the window to be repainted. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_invalidateRect"><span class="SCde"><span class="SFct">invalidateRect</span>()</spa</a></td>
<td>Force o local position to be repainted. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_isEnabled"><span class="SCde"><span class="SFct">isEnabled</span>()</spa</a></td>
<td>Returns true if window, and all its parents, are enabled. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_isParentOf"><span class="SCde"><span class="SFct">isParentOf</span>()</spa</a></td>
<td>Returns true if <span class="inline-code">child</span> is in the child hieararchy of <span class="inline-code">self</span>. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_isVisible"><span class="SCde"><span class="SFct">isVisible</span>()</spa</a></td>
<td>Returns true if window, and all its parents, are visible. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_isVisibleState"><span class="SCde"><span class="SFct">isVisibleState</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_localToSurface"><span class="SCde"><span class="SFct">localToSurface</span>(<span class="SKwd">self</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Point</span>)</span></a></td>
<td>Convert a local coordinate to a surface coordinate. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_localToSurface"><span class="SCde"><span class="SFct">localToSurface</span>(<span class="SKwd">self</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Rectangle</span>)</span></a></td>
<td>Convert a local coordinate to a surface coordinate. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_move"><span class="SCde"><span class="SFct">move</span>()</spa</a></td>
<td>Move the wnd. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_notifyEvent"><span class="SCde"><span class="SFct">notifyEvent</span>()</spa</a></td>
<td>Send a notification event. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_ownerNotifyEvent"><span class="SCde"><span class="SFct">ownerNotifyEvent</span>()</spa</a></td>
<td>Send a notification event. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_paint"><span class="SCde"><span class="SFct">paint</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_postCommandEvent"><span class="SCde"><span class="SFct">postCommandEvent</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_postEvent"><span class="SCde"><span class="SFct">postEvent</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_postInvalidateEvent"><span class="SCde"><span class="SFct">postInvalidateEvent</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_postQuitEvent"><span class="SCde"><span class="SFct">postQuitEvent</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_postResizeEvent"><span class="SCde"><span class="SFct">postResizeEvent</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_processEvent"><span class="SCde"><span class="SFct">processEvent</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_registerAction"><span class="SCde"><span class="SFct">registerAction</span>()</spa</a></td>
<td>Register one new action. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_registerKeyShortcut"><span class="SCde"><span class="SFct">registerKeyShortcut</span>()</spa</a></td>
<td>Register a keyboard shortcut. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_releaseMouse"><span class="SCde"><span class="SFct">releaseMouse</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_resize"><span class="SCde"><span class="SFct">resize</span>()</spa</a></td>
<td>Resize the wnd. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_screenToSurface"><span class="SCde"><span class="SFct">screenToSurface</span>()</spa</a></td>
<td>Convert a screen coordinate to a surface coordinate (relative to my surface). </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_sendCommandEvent"><span class="SCde"><span class="SFct">sendCommandEvent</span>()</spa</a></td>
<td>Send the command event with the given id to the window. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_sendComputeCommandStateEvent"><span class="SCde"><span class="SFct">sendComputeCommandStateEvent</span>()</spa</a></td>
<td>Send the command state event with the given id to the window. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_sendCreateEvent"><span class="SCde"><span class="SFct">sendCreateEvent</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_sendEvent"><span class="SCde"><span class="SFct">sendEvent</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_sendResizeEvent"><span class="SCde"><span class="SFct">sendResizeEvent</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_sendStateEvent"><span class="SCde"><span class="SFct">sendStateEvent</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_serializeState"><span class="SCde"><span class="SFct">serializeState</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_setAllMargins"><span class="SCde"><span class="SFct">setAllMargins</span>(<span class="SKwd">self</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Vector4</span>)</span></a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_setAllMargins"><span class="SCde"><span class="SFct">setAllMargins</span>(<span class="SKwd">self</span>, <span class="STpe">f32</span>)</span></a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_setAllPaddings"><span class="SCde"><span class="SFct">setAllPaddings</span>(<span class="SKwd">self</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Vector4</span>)</span></a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_setAllPaddings"><span class="SCde"><span class="SFct">setAllPaddings</span>(<span class="SKwd">self</span>, <span class="STpe">f32</span>)</span></a></td>
<td></td>
</tr>
<tr>
<td><a href="#Gui_Wnd_setFocus"><span class="SCde"><span class="SFct">setFocus</span>()</spa</a></td>
<td>Set the keyboard focus. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_setMargin"><span class="SCde"><span class="SFct">setMargin</span>()</spa</a></td>
<td>Set margin for childs  If a value is Swag.F32.Inf, then it won't be changed. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_setPadding"><span class="SCde"><span class="SFct">setPadding</span>()</spa</a></td>
<td>Set padding for childs  If a value is Swag.F32.Inf, then it won't be changed. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_setParent"><span class="SCde"><span class="SFct">setParent</span>()</spa</a></td>
<td>Set window parent. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_setPosition"><span class="SCde"><span class="SFct">setPosition</span>(<span class="SKwd">self</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Rectangle</span>, <span class="STpe">bool</span>)</span></a></td>
<td>Move and size the wnd. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_setPosition"><span class="SCde"><span class="SFct">setPosition</span>(<span class="SKwd">self</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">bool</span>)</span></a></td>
<td>Move and size the wnd. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_show"><span class="SCde"><span class="SFct">show</span>()</spa</a></td>
<td>Show the window. </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_surfaceToLocal"><span class="SCde"><span class="SFct">surfaceToLocal</span>()</spa</a></td>
<td>Convert a surface coordinate to a local coordinate (relative to me). </td>
</tr>
<tr>
<td><a href="#Gui_Wnd_updateCommandState"><span class="SCde"><span class="SFct">updateCommandState</span>()</spa</a></td>
<td>Main function to update command state of various windows/widgets. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_IWnd_onApplyStateEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onApplyStateEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L205" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onApplyStateEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_CommandStateEvent">CommandStateEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_IWnd_onCommandEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onCommandEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L203" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onCommandEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_CommandEvent">CommandEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_IWnd_onComputeStateEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onComputeStateEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L204" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onComputeStateEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_CommandStateEvent">CommandStateEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_IWnd_onCreateEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onCreateEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L185" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onCreateEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_CreateEvent">CreateEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_IWnd_onDestroyEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onDestroyEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L186" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onDestroyEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_DestroyEvent">DestroyEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_IWnd_onEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L134" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Event">Event</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_IWnd_onFocusEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onFocusEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L200" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onFocusEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_FocusEvent">FocusEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_IWnd_onFrameEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onFrameEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L196" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onFrameEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_FrameEvent">FrameEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_IWnd_onHookEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onHookEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L194" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onHookEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Event">Event</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_IWnd_onKeyEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onKeyEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L201" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onKeyEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_KeyEvent">KeyEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_IWnd_onMouseEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onMouseEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L202" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onMouseEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_MouseEvent">MouseEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_IWnd_onNotifyEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onNotifyEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L206" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onNotifyEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_NotifyEvent">NotifyEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_IWnd_onPaintEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onPaintEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L190" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onPaintEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_PaintEvent">PaintEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_IWnd_onPostPaintEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onPostPaintEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L191" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onPostPaintEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_PaintEvent">PaintEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_IWnd_onPrePaintEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onPrePaintEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L189" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onPrePaintEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_PaintEvent">PaintEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_IWnd_onResizeEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onResizeEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L188" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onResizeEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ResizeEvent">ResizeEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_IWnd_onSerializeStateEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onSerializeStateEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L195" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onSerializeStateEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SerializeStateEvent">SerializeStateEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_IWnd_onSetThemeEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onSetThemeEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L192" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onSetThemeEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SetThemeEvent">SetThemeEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_IWnd_onStateEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onStateEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L187" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onStateEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_StateEvent">StateEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_IWnd_onSysCommandEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onSysCommandEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L198" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onSysCommandEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SysCommandEvent">SysCommandEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_IWnd_onSysUserEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onSysUserEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L199" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onSysUserEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SysUserEvent">SysUserEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_IWnd_onTimerEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onTimerEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L193" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onTimerEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_TimerEvent">TimerEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_applyLayout"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">applyLayout</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L784" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Apply the current layout (childs constraints). </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">applyLayout</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">applyLayout</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, oldSize: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Point</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_bringToFront"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">bringToFront</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L691" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Force the window to be the first painted (before all siblings). </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">bringToFront</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_bringToTop"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">bringToTop</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L705" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Force the window to be the last painted (on top of siblings). </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">bringToTop</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_captureMouse"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">captureMouse</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L980" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">captureMouse</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_computeStyle"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">computeStyle</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L432" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">computeStyle</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_create"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">create</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L229" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span>(<span class="SCst">T</span>) <span class="SFct">create</span>(parent: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span> = <span class="SKwd">null</span>, id: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span> = <span class="SKwd">null</span>, hook: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_HookEvent">HookEvent</a></span> = <span class="SKwd">null</span>)-&gt;*<span class="SCst">T</span>
<span class="SKwd">func</span>(<span class="SCst">T</span>) <span class="SFct">create</span>(parent: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>, position: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span>, id: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span> = <span class="SKwd">null</span>, hook: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_HookEvent">HookEvent</a></span> = <span class="SKwd">null</span>)-&gt;*<span class="SCst">T</span>
<span class="SKwd">func</span>(<span class="SCst">T</span>) <span class="SFct">create</span>(parent: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>, name: <span class="STpe">string</span>, position: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span>, id: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span> = <span class="SKwd">null</span>, hook: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_HookEvent">HookEvent</a></span> = <span class="SKwd">null</span>)-&gt;*<span class="SCst">T</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_destroy"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">destroy</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L270" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Destroy the window. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">destroy</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_destroyNow"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">destroyNow</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L263" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Destroy the window. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">destroyNow</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_disable"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">disable</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L391" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Disable the window. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">disable</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_enable"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">enable</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L368" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Enable/Disable the window. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">enable</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, state = <span class="SKwd">true</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_fitPosInParent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">fitPosInParent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L548" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Be sure rect is fully inside the parent rectangle (if possible). </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fitPosInParent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, marginW: <span class="STpe">f32</span> = <span class="SNum">0</span>, marginH: <span class="STpe">f32</span> = <span class="SNum">0</span>, simX = <span class="SKwd">false</span>, simY = <span class="SKwd">false</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_getApp"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">getApp</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L217" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getApp</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Application">Application</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_getChildById"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">getChildById</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L948" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Retrieve the child with the given id. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getChildById</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, childId: <span class="STpe">string</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_getClientRect"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">getClientRect</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L599" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the client area. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getClientRect</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_getClientRectPadding"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">getClientRectPadding</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L605" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the client area, with <span class="inline-code">padding</span> applied. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getClientRectPadding</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_getFocus"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">getFocus</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L1029" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the window with the keyboard focus. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getFocus</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_getFont"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">getFont</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L225" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getFont</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst">FontFamily</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_getKeyShortcut"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">getKeyShortcut</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L1053" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the id associated with a shortcut. null if none. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getKeyShortcut</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, mdf: <span class="SCst">Core</span>.<span class="SCst">Input</span>.<span class="SCst">KeyModifiers</span>, key: <span class="SCst">Core</span>.<span class="SCst">Input</span>.<span class="SCst">Key</span>)-&gt;<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_getKeyShortcutNameFor"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">getKeyShortcutNameFor</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L1067" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the name associated with a given id shortcut. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getKeyShortcutNameFor</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, wndId: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span>)-&gt;<span class="SCst">Core</span>.<span class="SCst">String</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_getMouseCapture"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">getMouseCapture</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L990" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getMouseCapture</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_getOwner"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">getOwner</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L333" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the owner of the window. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getOwner</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_getParentById"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">getParentById</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L963" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Retrieve the parent with the given id. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getParentById</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, parentId: <span class="STpe">string</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_getRectIn"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">getRectIn</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L631" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the wnd position relative to a given parent. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getRectIn</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, wnd: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>)-&gt;<span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_getSurfaceRect"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">getSurfaceRect</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L614" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the wnd position in the surface. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getSurfaceRect</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_getTheme"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">getTheme</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L220" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getTheme</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Theme">Theme</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_getThemeColors"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">getThemeColors</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L222" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getThemeColors</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeColors">ThemeColors</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_getThemeMetrics"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">getThemeMetrics</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L221" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getThemeMetrics</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeMetrics">ThemeMetrics</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_getThemeRects"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">getThemeRects</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L223" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getThemeRects</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ThemeImageRects">ThemeImageRects</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_getTiming"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">getTiming</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L224" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getTiming</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;*<span class="SCst">Core</span>.<span class="SCst">Time</span>.<span class="SCst">FrameTiming</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_getTopView"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">getTopView</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L218" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getTopView</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_getTopWnd"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">getTopWnd</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L219" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getTopWnd</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_SurfaceWnd">SurfaceWnd</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_getWndAt"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">getWndAt</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L903" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the child window at the given coordinate  Coordinate is expressed in the parent system. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getWndAt</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, x: <span class="STpe">f32</span>, y: <span class="STpe">f32</span>, getDisabled = <span class="SKwd">false</span>, getHidden = <span class="SKwd">false</span>)-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_hasFocus"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">hasFocus</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L1035" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns true if the window has the keyboard focus. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">hasFocus</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_hide"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">hide</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L358" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Hide the window. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">hide</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_invalidate"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">invalidate</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L528" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Force the window to be repainted. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">invalidate</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_invalidateRect"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">invalidateRect</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L538" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Force o local position to be repainted. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">invalidateRect</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, rect: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_isEnabled"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">isEnabled</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L415" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns true if window, and all its parents, are enabled. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">isEnabled</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_isParentOf"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">isParentOf</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L932" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns true if <span class="inline-code">child</span> is in the child hieararchy of <span class="inline-code">self</span>. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">isParentOf</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, child: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_isVisible"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">isVisible</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L397" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns true if window, and all its parents, are visible. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">isVisible</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, onlyMe = <span class="SKwd">false</span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_isVisibleState"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">isVisibleState</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L412" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">isVisibleState</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_localToSurface"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">localToSurface</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L585" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Convert a local coordinate to a surface coordinate. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">localToSurface</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, pos: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Point</span>)-&gt;<span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Point</span></span></code>
</div>
<p>Convert a local coordinate to a surface coordinate. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">localToSurface</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, pos: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span>)-&gt;<span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_move"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">move</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L742" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Move the wnd. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">move</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, x: <span class="STpe">f32</span>, y: <span class="STpe">f32</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_notifyEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">notifyEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L1163" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Send a notification event. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">notifyEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, kind: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_NotifyEvent">NotifyEvent</a></span>.<span class="SCst">Kind</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_ownerNotifyEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">ownerNotifyEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L1171" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Send a notification event. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">ownerNotifyEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, kind: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_NotifyEvent">NotifyEvent</a></span>.<span class="SCst">Kind</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_paint"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">paint</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L438" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">paint</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, bc: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_PaintContext">PaintContext</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_postCommandEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">postCommandEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L1143" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">postCommandEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, cmdId: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_postEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">postEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L1093" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">postEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Event">Event</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_postInvalidateEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">postInvalidateEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L1099" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">postInvalidateEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_postQuitEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">postQuitEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L1088" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">postQuitEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_postResizeEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">postResizeEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L1126" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">postResizeEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_processEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">processEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L1153" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">processEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Event">Event</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_registerAction"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">registerAction</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L1253" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Register one new action. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span>(<span class="SCst">T</span>) <span class="SFct">registerAction</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_registerKeyShortcut"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">registerKeyShortcut</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L1038" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Register a keyboard shortcut. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">registerKeyShortcut</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, mdf: <span class="SCst">Core</span>.<span class="SCst">Input</span>.<span class="SCst">KeyModifiers</span>, key: <span class="SCst">Core</span>.<span class="SCst">Input</span>.<span class="SCst">Key</span>, wndId: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_releaseMouse"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">releaseMouse</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L985" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">releaseMouse</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_resize"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">resize</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L766" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Resize the wnd. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">resize</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, width: <span class="STpe">f32</span>, height: <span class="STpe">f32</span>, computeLayout = <span class="SKwd">true</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_screenToSurface"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">screenToSurface</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L572" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Convert a screen coordinate to a surface coordinate (relative to my surface). </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">screenToSurface</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, pos: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Point</span>)-&gt;<span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Point</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_sendCommandEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">sendCommandEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L1228" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Send the command event with the given id to the window. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">sendCommandEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, cmdId: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span>)</span></code>
</div>
<p> The command will be updated first, and if it is not disabled, it will be sent. </p>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_sendComputeCommandStateEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">sendComputeCommandStateEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L1200" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Send the command state event with the given id to the window. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">sendComputeCommandStateEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, cmdId: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WndId">WndId</a></span>)-&gt;<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_CommandStateEvent">CommandStateEvent</a></span></span></code>
</div>
<p> Returns the updated command state. </p>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_sendCreateEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">sendCreateEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L1134" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">sendCreateEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_sendEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">sendEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L1106" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">sendEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Event">Event</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_sendResizeEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">sendResizeEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L1118" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">sendResizeEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_sendStateEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">sendStateEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L1112" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">sendStateEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, kind: <span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_EventKind">EventKind</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_serializeState"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">serializeState</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L1301" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">serializeState</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, ser: *<span class="SCst">Core</span>.<span class="SCst">Serialization</span>.<span class="SCst">Serializer</span>) <span class="SKwd">throw</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_setAllMargins"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">setAllMargins</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L666" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setAllMargins</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, value: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Vector4</span>)
<span class="SKwd">func</span> <span class="SFct">setAllMargins</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, value: <span class="STpe">f32</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_setAllPaddings"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">setAllPaddings</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L688" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setAllPaddings</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, value: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Vector4</span>)
<span class="SKwd">func</span> <span class="SFct">setAllPaddings</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, value: <span class="STpe">f32</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_setFocus"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">setFocus</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L1000" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the keyboard focus. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setFocus</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_setMargin"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">setMargin</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L647" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set margin for childs  If a value is Swag.F32.Inf, then it won't be changed. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setMargin</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, left: <span class="STpe">f32</span> = <span class="SCst">Swag</span>.<span class="SCst">F32</span>.<span class="SCst">Inf</span>, top: <span class="STpe">f32</span> = <span class="SCst">Swag</span>.<span class="SCst">F32</span>.<span class="SCst">Inf</span>, right: <span class="STpe">f32</span> = <span class="SCst">Swag</span>.<span class="SCst">F32</span>.<span class="SCst">Inf</span>, bottom: <span class="STpe">f32</span> = <span class="SCst">Swag</span>.<span class="SCst">F32</span>.<span class="SCst">Inf</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_setPadding"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">setPadding</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L670" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set padding for childs  If a value is Swag.F32.Inf, then it won't be changed. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setPadding</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, left: <span class="STpe">f32</span> = <span class="SCst">Swag</span>.<span class="SCst">F32</span>.<span class="SCst">Inf</span>, top: <span class="STpe">f32</span> = <span class="SCst">Swag</span>.<span class="SCst">F32</span>.<span class="SCst">Inf</span>, right: <span class="STpe">f32</span> = <span class="SCst">Swag</span>.<span class="SCst">F32</span>.<span class="SCst">Inf</span>, bottom: <span class="STpe">f32</span> = <span class="SCst">Swag</span>.<span class="SCst">F32</span>.<span class="SCst">Inf</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_setParent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">setParent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L297" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set window parent. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setParent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, who: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_setPosition"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">setPosition</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L728" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Move and size the wnd. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setPosition</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, rect: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span>, computeLayout = <span class="SKwd">true</span>)</span></code>
</div>
<p>Move and size the wnd. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setPosition</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, x: <span class="STpe">f32</span>, y: <span class="STpe">f32</span>, w: <span class="STpe">f32</span>, h: <span class="STpe">f32</span>, computeLayout = <span class="SKwd">true</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_show"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">show</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L343" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Show the window. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">show</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, b = <span class="SKwd">true</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_surfaceToLocal"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">surfaceToLocal</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L578" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Convert a surface coordinate to a local coordinate (relative to me). </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">surfaceToLocal</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, pos: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Point</span>)-&gt;<span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Point</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_Wnd_updateCommandState"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Wnd.</span><span class="api-item-title-strong">updateCommandState</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L1184" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Main function to update command state of various windows/widgets. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">updateCommandState</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p> This will update the state of this window, and all of its childs </p>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_WndFlags"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">WndFlags</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\wnd\wnd.swg#L77" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCst">Zero</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">NoScroll</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">ClipChildren</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Disabled</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">Hidden</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">PreChildsPaint</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">PostChildsPaint</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCst">TopMost</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_WrapLayoutCtrl"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gui.</span><span class="api-item-title-strong">WrapLayoutCtrl</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\wraplayoutctrl.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SCde"><span class="SKwd">using</span> wnd</span></td>
<td class="code-type"><span class="SCde"><span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">spacing</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">wrapWidth</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">wrapHeight</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SCde">resultHeight</span></td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td><a href="#Gui_WrapLayoutCtrl_computeLayout"><span class="SCde"><span class="SFct">computeLayout</span>()</spa</a></td>
<td>Recompute layout of all childs. </td>
</tr>
<tr>
<td><a href="#Gui_WrapLayoutCtrl_create"><span class="SCde"><span class="SFct">create</span>()</spa</a></td>
<td>Create a layout control. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_WrapLayoutCtrl_IWnd_onResizeEvent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IWnd.</span><span class="api-item-title-strong">onResizeEvent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\wraplayoutctrl.swg#L16" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onResizeEvent</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, evt: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_ResizeEvent">ResizeEvent</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_WrapLayoutCtrl_computeLayout"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">WrapLayoutCtrl.</span><span class="api-item-title-strong">computeLayout</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\wraplayoutctrl.swg#L57" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Recompute layout of all childs. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">computeLayout</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Gui_WrapLayoutCtrl_create"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">WrapLayoutCtrl.</span><span class="api-item-title-strong">create</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gui\src\composite\wraplayoutctrl.swg#L50" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Create a layout control. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">create</span>(parent: *<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_Wnd">Wnd</a></span>, position: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span> = {})-&gt;*<span class="SCst">Gui</span>.<span class="SCst"><a href="#Gui_WrapLayoutCtrl">WrapLayoutCtrl</a></span></span></code>
</div>
</div>
</div>
</div>
</body>
</html>
