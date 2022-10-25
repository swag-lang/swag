enum EditMode
{
    Shape
    Arrow
    Line
    Polygon
    Text
    Favorites
    Draw
    Highlight
    Selection
}

enum BottomSize
{
    Small
    Medium
    Big
}

struct MainWnd
{
    using wnd: Wnd

    icons24:        ImageList
    icons48:        ImageList
    editToolBar:    *Wnd
    editWnd:        *EditWnd
    quickWnd:       *QuickWnd
    propWnd:        *PropWnd
    libWnd:         *LibraryWnd
    clipFmt:        Clipboard.Format
    captureBtn:     *IconButton
    libraryBtn:     *IconButton
    zoomBtn:        *IconButton
    recentWnd:      *RecentWnd
    recentBar:      *Wnd
    recentSizeBar:  *IconBar
    quickStyleBar:  *SplitterCtrl
    editZoomBar:    *StackLayoutCtrl
    captureMode:    CaptureMode = .Area
    monitors:       Array'Env.Monitor

    editMode:       EditMode = .Shape
    lastEditMode:   EditMode = .Shape
    bottomSize:     BottomSize = Medium
    snap:           bool
    libraryMode:    bool
    hideCaptMe      = true
    fileDlgState:   FileDlgState

    mdlFormShape:   FormShape
    mdlFormArrow:   FormArrow
    mdlFormLine:    FormLine
    mdlFormPolygon: FormPolygon
    mdlFormText:    FormText
    mdlFormDraw:    FormDraw
    mdlHighlight:   IForm
    mdlFavorite:    IForm
}

const Icons24 = #include("datas/icons24.png")
const Icons48 = #include("datas/icons48.png")

impl IWnd for MainWnd
{
    mtd onSysCommandEvent(evt: *SysCommandEvent)
    {
        evt.accepted = false
        if evt.sysKind == .Close
        {
            libWnd.requestClose()
        }
    }

    mtd onSerializeStateEvent(evt: *SerializeStateEvent)
    {
        catch
        {
            with evt.ser
            {
                .serialize("MainWnd.editMode", &editMode)
                .serialize("MainWnd.snap", &snap)
                .serialize("MainWnd.bottomSize", &bottomSize)
                .serialize("MainWnd.fileDlgState", &fileDlgState)
                .serialize("MainWnd.hideCaptMe", &hideCaptMe)
                .serialize("MainWnd.captureMode", &captureMode)
                .serialize("MainWnd.RightBar.Pane0", &quickStyleBar.items[0].size)
                .serialize("MainWnd.RightBar.Pane1", &quickStyleBar.items[1].size)
            }
        }

        if evt.ser.isReading()
        {
            setBottomSize(bottomSize)
            updateState()
        }
    }

    mtd onCommandUIEvent(evt: *CommandUIEvent)
    {
        editView := editWnd  ? cast(*EditView) editWnd.view : null
        capture  := editView and !libraryMode ? editView.capture : null

        hasSel       := capture and capture.selection.count
        hasSingleSel := capture and capture.selection.count == 1

        switch evt.id
        {
        case CmdIdMoveSelLeft, CmdIdMoveSelTop, CmdIdMoveSelRight, CmdIdMoveSelBottom:
            evt.setFlags |= .Disabled
            evt.disabled = !hasSel
            evt.accepted = true
        case CmdIdMoveSelLeft1, CmdIdMoveSelTop1, CmdIdMoveSelRight1, CmdIdMoveSelBottom1:
            evt.setFlags |= .Disabled
            evt.disabled = !hasSel
            evt.accepted = true

        case CmdIdCaptureHideEditor:
            evt.setFlags |= .Name | .Checked
            evt.name = "Hide Editor On Capture"
            evt.checked = hideCaptMe
            evt.accepted = true

        case CmdIdSaveAsImage:
            evt.setFlags |= .Name | .Disabled
            evt.name = "Save As Image..."
            evt.disabled = !capture
            evt.accepted = true

        case CmdIdOpenImage:
            evt.setFlags |= .Name
            evt.name = "Open Image..."
            evt.accepted = true

        case CmdIdNewImage:
            evt.setFlags |= .Name
            evt.name = "New Image"
            evt.accepted = true

        case CmdIdNewFromClipboard:
            evt.setFlags |= .Name | .Disabled
            evt.name = "New From Clipboard"
            evt.disabled = !Clipboard.hasImage()
            evt.accepted = true

        case CmdIdExit:
            evt.setFlags |= .Name
            evt.name = "Exit"
            evt.accepted = true

        case CmdIdRestoreOrg:
            evt.setFlags |= .Name | .Disabled
            evt.name = "Restore to Original"
            evt.disabled = !capture or !capture.backImgOrg.isValid()
            evt.accepted = true

        case CmdIdResetQuickStyle:
            evt.setFlags |= .Name
            evt.name = "Restore Default Styles"
            evt.accepted = true

        case CmdIdRecentSmall:
            evt.setFlags |= .Name | .ToolTip | .Checked
            evt.toolTip = "Small recent view"
            evt.checked = bottomSize == .Small
            evt.accepted = true

        case CmdIdRecentMedium:
            evt.setFlags |= .Name | .ToolTip | .Checked
            evt.toolTip = "Medium recent view"
            evt.checked = bottomSize == .Medium
            evt.accepted = true

        case CmdIdRecentBig:
            evt.setFlags |= .Name | .ToolTip | .Checked
            evt.toolTip = "Big recent view"
            evt.checked = bottomSize == .Big
            evt.accepted = true

        case CmdIdDeleteCapture:
            evt.setFlags |= .Name | .Disabled
            evt.name = "Delete"
            evt.disabled = !capture
            evt.accepted = true

        case CmdIdFlatten:
            evt.setFlags |= .Name | .Disabled
            evt.name = "Flatten"
            evt.disabled = !capture or !hasSel
            evt.accepted = true

        case CmdIdFlattenAll:
            evt.setFlags |= .Name | .Disabled
            evt.name = "Flatten All"
            evt.disabled = !capture or !capture.forms.count
            evt.accepted = true

        case CmdIdFileLocation:
            evt.setFlags |= .Name | .Disabled
            evt.name = "Open File Location"
            evt.disabled = !capture
            evt.accepted = true

        case CmdIdZoom1:
            evt.setFlags |= .Name | .Disabled
            evt.name = "Actual Size"
            evt.disabled = !capture
            evt.accepted = true

        case CmdIdZoomFit:
            evt.setFlags |= .Name | .Disabled
            evt.name = "Fit To View"
            evt.disabled = !capture
            evt.accepted = true

        case CmdIdZoomIn:
            evt.setFlags |= .Name | .Disabled
            evt.name = "Zoom In"
            evt.disabled = !capture or !editWnd.editView.canZoomIn()
            evt.accepted = true

        case CmdIdZoomOut:
            evt.setFlags |= .Name | .Disabled
            evt.name = "Zoom Out"
            evt.disabled = !capture or !editWnd.editView.canZoomOut()
            evt.accepted = true

        case CmdIdDupCapture:
            evt.setFlags |= .Name | .Disabled
            evt.name = "Duplicate"
            evt.disabled = !capture
            evt.accepted = true

        case CmdIdSnap:
            evt.setFlags |= .Name | .Checked
            evt.name = "Snap To Forms"
            evt.checked = snap
            evt.accepted = true

        case CmdIdCaptModeArea:
            evt.setFlags |= .Name | .Checked
            evt.name = "Area"
            evt.checked = captureMode == .Area
            evt.accepted = true

        case CmdIdCaptModeLastArea:
            evt.setFlags |= .Name | .Checked
            evt.name = "Last Area"
            evt.checked = captureMode == .LastArea
            evt.accepted = true

        case CmdIdCaptModeFullScreen:
            evt.setFlags |= .Name | .Checked
            evt.name = "Full Screen"
            evt.checked = captureMode == .FullScreen
            evt.accepted = true

        case CmdIdCaptureArea:
            evt.setFlags |= .Name | .Icon
            evt.name = "Capture Area"
            evt.icon = Icon.from(&icons24, 7)
            evt.accepted = true
        case CmdIdCaptureFullScreen:
            evt.setFlags |= .Name | .Icon
            evt.name = "Capture Full Screen"
            evt.icon = Icon.from(&icons24, 6)
            evt.accepted = true
        case CmdIdCaptureActiveWnd:
            evt.setFlags |= .Name | .Icon
            evt.name = "Capture Active Window"
            evt.icon = Icon.from(&icons24, 14)
            evt.accepted = true
        case CmdIdCaptureScreen0:
            evt.setFlags |= .Name | .Icon
            evt.name = Format.toString("Capture Monitor 0 (%x%)", monitors[0].rect.width, monitors[0].rect.height)
            evt.icon = Icon.from(&icons24, 5)
            evt.accepted = true
        case CmdIdCaptureScreen1:
            evt.setFlags |= .Name | .Icon
            evt.name = Format.toString("Capture Monitor 1 (%x%)", monitors[1].rect.width, monitors[1].rect.height)
            evt.icon = Icon.from(&icons24, 5)
            evt.accepted = true
        case CmdIdCaptureScreen2:
            evt.setFlags |= .Name | .Icon
            evt.name = Format.toString("Capture Monitor 2 (%x%)", monitors[2].rect.width, monitors[2].rect.height)
            evt.icon = Icon.from(&icons24, 5)
            evt.accepted = true
        case CmdIdCaptureScreen3:
            evt.setFlags |= .Name | .Icon
            evt.name = Format.toString("Capture Monitor 3 (%x%)", monitors[3].rect.width, monitors[3].rect.height)
            evt.icon = Icon.from(&icons24, 5)
            evt.accepted = true

        case CmdIdCapture:
            evt.setFlags |= .Name | .LongName | .ToolTip | .Icon
            evt.name = "Capture"
            evt.longName = "Capture"

            switch captureMode
            {
            case Area:          evt.longName += " (Area)"
            case LastArea:      evt.longName += " (Last Area)"
            case FullScreen:    evt.longName += " (Full Screen)"
            default:            break
            }

            evt.toolTip = "Start a new capture"
            evt.icon = Icon.from(&icons48, 6)
            evt.accepted = true

        case CmdIdLibrary:
            evt.setFlags |= .Name | .ToolTip | .Checked
            evt.name = "Library"
            evt.toolTip = "Display captures library"
            evt.accepted = true
            evt.checked = libraryMode

        case CmdIdSelectAll:
            evt.setFlags |= .Disabled | .Name
            evt.disabled = !capture or !capture.forms.count
            evt.name = "Select All Objects"
            evt.accepted = true

        case CmdIdDeSelectAll:
            evt.setFlags |= .Disabled | .Name
            evt.disabled = !capture or !hasSel
            evt.name = "Deselect All Objects"
            evt.accepted = true

        case CmdIdInvertSelection:
            evt.setFlags |= .Disabled | .Name
            evt.disabled = !capture
            evt.name = "Invert Selection"
            evt.accepted = true

        case CmdIdCopy:
            evt.setFlags |= .Disabled | .Name
            evt.disabled = !hasSel
            evt.name = "Copy"
            evt.accepted = true

        case CmdIdPaste:
            evt.setFlags |= .Disabled | .Name
            evt.disabled = !capture or (!Clipboard.hasFormat(clipFmt) and !Clipboard.hasImage())
            evt.name = "Paste"
            evt.accepted = true

        case CmdIdCopyAsImage:
            evt.setFlags |= .Disabled | .Name
            evt.disabled = capture == null
            evt.name = "Copy As Image"
            evt.accepted = true

        case CmdIdDeleteSelection:
            evt.setFlags |= .Disabled | .Name
            evt.disabled = !hasSel
            evt.name = "Delete"
            evt.accepted = true

        case CmdIdModeShape:
            evt.setFlags |= .Icon | .Checked | .Name | .ToolTip
            evt.icon = Icon.from(&icons48, 0)
            evt.name = "Shape"
            evt.toolTip = "Add circles and rectangle to the capture"
            evt.checked = editMode == .Shape
            evt.accepted = true

        case CmdIdModeArrow:
            evt.setFlags |= .Icon | .Checked | .Name | .ToolTip
            evt.icon = Icon.from(&icons48, 3)
            evt.name = "Arrow"
            evt.toolTip = "Add arrows to the capture"
            evt.checked = editMode == .Arrow
            evt.accepted = true

        case CmdIdModeLine:
            evt.setFlags |= .Icon | .Checked | .Name | .ToolTip
            evt.icon = Icon.from(&icons48, 1)
            evt.name = "Line"
            evt.toolTip = "Add lines to the capture"
            evt.checked = editMode == .Line
            evt.accepted = true

        case CmdIdModePolygon:
            evt.setFlags |= .Icon | .Checked | .Name | .ToolTip
            evt.icon = Icon.from(&icons48, 2)
            evt.name = "Polygon"
            evt.toolTip = "Add polygons to the capture"
            evt.checked = editMode == .Polygon
            evt.accepted = true

        case CmdIdModeText:
            evt.setFlags |= .Icon | .Checked | .Name | .ToolTip
            evt.icon = Icon.from(&icons48, 4)
            evt.name = "Text"
            evt.toolTip = "Add some text to the capture"
            evt.checked = editMode == .Text
            evt.accepted = true

        case CmdIdModeDraw:
            evt.setFlags |= .Icon | .Checked | .Name | .ToolTip
            evt.icon = Icon.from(&icons48, 7)
            evt.name = "Draw"
            evt.toolTip = "Add some free hand paint to the capture"
            evt.checked = editMode == .Draw
            evt.accepted = true

        case CmdIdModeHighlight:
            evt.setFlags |= .Icon | .Checked | .Name | .ToolTip
            evt.icon = Icon.from(&icons48, 9)
            evt.name = "Highlight"
            evt.toolTip = "Highlight some parts of the capture"
            evt.checked = editMode == .Highlight
            evt.accepted = true

        case CmdIdModeFavorites:
            evt.setFlags |= .Icon | .Checked | .Name | .ToolTip
            evt.icon = Icon.from(&icons48, 5)
            evt.name = "Favorites"
            evt.toolTip = "Save multiple quick styles in one single location"
            evt.checked = editMode == .Favorites
            evt.accepted = true

        case CmdIdModeSelection:
            evt.setFlags |= .Icon | .Checked | .Name | .ToolTip
            evt.icon = Icon.from(&icons48, 10)
            evt.name = "Select"
            evt.toolTip = "Select forms"
            evt.checked = editMode == .Selection or (capture and capture.drawSelRect)
            evt.accepted = true

        case CmdIdAlignLeft:
            evt.setFlags |= .Disabled | .Name
            evt.name = "Left"
            evt.disabled = !hasSel or hasSingleSel
            evt.accepted = true

        case CmdIdAlignCenter:
            evt.setFlags |= .Disabled | .Name
            evt.name = "Center"
            evt.disabled = !hasSel or hasSingleSel
            evt.accepted = true

        case CmdIdAlignRight:
            evt.setFlags |= .Disabled | .Name
            evt.name = "Right"
            evt.disabled = !hasSel or hasSingleSel
            evt.accepted = true

        case CmdIdAlignTop:
            evt.setFlags |= .Disabled | .Name
            evt.name = "Top"
            evt.disabled = !hasSel or hasSingleSel
            evt.accepted = true

        case CmdIdAlignMiddle:
            evt.setFlags |= .Disabled | .Name
            evt.name = "Middle"
            evt.disabled = !hasSel or hasSingleSel
            evt.accepted = true

        case CmdIdAlignBottom:
            evt.setFlags |= .Disabled | .Name
            evt.name = "Bottom"
            evt.disabled = !hasSel or hasSingleSel
            evt.accepted = true

        case CmdIdBringToFront:
            evt.setFlags |= .Disabled | .Icon | .Name
            evt.name = "Bring To Front"
            evt.disabled = !hasSel or capture.selection.contains(capture.forms.count - 1)
            evt.icon = evt.target.getTheme().getIcon24(.BringToFront)
            evt.accepted = true

        case CmdIdSendToBack:
            evt.setFlags |= .Disabled | .Icon | .Name
            evt.name = "Send To Back"
            evt.disabled = !hasSel or capture.selection.contains(0)
            evt.icon = evt.target.getTheme().getIcon24(.SendToBack)
            evt.accepted = true

        case CmdIdBringForward:
            evt.setFlags |= .Disabled | .Icon | .Name
            evt.name = "Bring Forward"
            evt.disabled = !hasSel or capture.selection.contains(capture.forms.count - 1)
            evt.icon = evt.target.getTheme().getIcon24(.BringForward)
            evt.accepted = true

        case CmdIdSendBackward:
            evt.setFlags |= .Disabled | .Icon | .Name
            evt.name = "Send Backward"
            evt.disabled = !hasSel or capture.selection.contains(0)
            evt.icon = evt.target.getTheme().getIcon24(.SendBackward)
            evt.accepted = true

        case CmdIdUndo:
            evt.setFlags |= .Disabled | .Name
            evt.name = "Undo"
            evt.disabled = !capture or !capture.canUndo()
            evt.accepted = true

        case CmdIdRedo:
            evt.setFlags |= .Disabled | .Name
            evt.name = "Redo"
            evt.disabled = !capture or !capture.canRedo()
            evt.accepted = true
        }
    }

    mtd onCommandEvent(evt: *CommandEvent)
    {
        editView := cast(*EditView) editWnd.view
        capture  := editView.capture

        var undo: *Undo
        switch evt.id
        {
        case CmdIdMoveSelLeft1:
            undo = capture.newUndo(.OffsetSelection)
            undo.newPosX = -1
            evt.accepted = true
        case CmdIdMoveSelTop1:
            undo = capture.newUndo(.OffsetSelection)
            undo.newPosY = -1
            evt.accepted = true
        case CmdIdMoveSelRight1:
            undo = capture.newUndo(.OffsetSelection)
            undo.newPosX = 1
            evt.accepted = true
        case CmdIdMoveSelBottom1:
            undo = capture.newUndo(.OffsetSelection)
            undo.newPosY = 1
            evt.accepted = true
        case CmdIdMoveSelLeft:
            undo = capture.newUndo(.OffsetSelection)
            undo.newPosX = -5
            evt.accepted = true
        case CmdIdMoveSelTop:
            undo = capture.newUndo(.OffsetSelection)
            undo.newPosY = -5
            evt.accepted = true
        case CmdIdMoveSelRight:
            undo = capture.newUndo(.OffsetSelection)
            undo.newPosX = 5
            evt.accepted = true
        case CmdIdMoveSelBottom:
            undo = capture.newUndo(.OffsetSelection)
            undo.newPosY = 5
            evt.accepted = true

        case CmdIdCaptModeArea:
            captureMode = .Area
        case CmdIdCaptModeLastArea:
            captureMode = .LastArea
        case CmdIdCaptModeFullScreen:
            captureMode = .FullScreen

        case CmdIdCaptureHideEditor:
            hideCaptMe = !hideCaptMe

        case CmdIdSaveAsImage:
            saveAsImage()
        case CmdIdOpenImage:
            openImage()

        case CmdIdExit:
            postQuitEvent()

        case CmdIdResetQuickStyle:
            quickWnd.resetQuickStyles()

        case CmdIdRecentSmall:
            setBottomSize(.Small)

        case CmdIdRecentMedium:
            setBottomSize(.Medium)

        case CmdIdRecentBig:
            setBottomSize(.Big)

        case CmdIdDeleteCapture:
            res := MessageDlg.confirm(surface, "This will delete the capture file.")
            if res == MessageDlg.BtnYes
                recentWnd.removeRecent()

        case CmdIdFileLocation:
            File.locateInExplorer(getCapture().file)

        case CmdIdZoom1:
            editWnd.editView.setZoom(1)

        case CmdIdZoomFit:
            editWnd.editView.zoomToFit()

        case CmdIdZoomIn:
            editWnd.editView.zoomIn()
        case CmdIdZoomOut:
            editWnd.editView.zoomOut()

        case CmdIdDupCapture:
            file := Library.newFileName()
            catch File.duplicate(file, capture.file)
            if @err break
            dupCapture := catch recentWnd.addRecent(file)
            if @err break
            Memory.delete(dupCapture)
            recentWnd.recentView.select(0)
            g_Library.addFile(file)

        case CmdIdSnap:
            snap = !snap

        case CmdIdNewImage:
            newImage()

        case CmdIdNewFromClipboard:
            newCapture(Clipboard.getImage())

        case CmdIdCaptureArea:
            captureScreen(.Area)
        case CmdIdCaptureFullScreen:
            captureScreen(.FullScreen)
        case CmdIdCaptureActiveWnd:
            captureScreen(.ActiveWindow)
        case CmdIdCaptureScreen0:
            captureScreen(.Screen0)
        case CmdIdCaptureScreen1:
            captureScreen(.Screen1)
        case CmdIdCaptureScreen2:
            captureScreen(.Screen2)
        case CmdIdCaptureScreen3:
            captureScreen(.Screen3)

        case CmdIdCapture:
            captureScreen(.None)

        case CmdIdLibrary:
            swapLibraryMode()

        case CmdIdSelectAll:
            capture.selection.clear()
            loop capture.forms
                capture.selection.add(@index)
            capture.selectionHasChanged()

        case CmdIdDeSelectAll:
            capture.selection.clear()
            capture.selectionHasChanged()

        case CmdIdInvertSelection:
            var newSel: Array'uint
            loop i: capture.forms
            {
                if !capture.selection.contains(i)
                    newSel.add(i)
            }
            capture.selection = newSel
            capture.selectionHasChanged()

        case CmdIdCopy:
            capture.copy()
        case CmdIdPaste:
            capture.paste()

        case CmdIdCopyAsImage:
            capture.copyAll()

        case CmdIdDeleteSelection:
            undo = capture.newUndo(.DeleteSelection)
            evt.accepted = true

        case CmdIdModeShape:
            editMode = .Shape
            evt.accepted = true
            updateState()

        case CmdIdModeArrow:
            editMode = .Arrow
            evt.accepted = true
            updateState()

        case CmdIdModeLine:
            editMode = .Line
            evt.accepted = true
            updateState()

        case CmdIdModePolygon:
            editMode = .Polygon
            evt.accepted = true
            updateState()

        case CmdIdModeText:
            editMode = .Text
            evt.accepted = true
            updateState()

        case CmdIdModeDraw:
            editMode = .Draw
            evt.accepted = true
            updateState()

        case CmdIdModeHighlight:
            editMode = .Highlight
            evt.accepted = true
            updateState()

        case CmdIdModeFavorites:
            editMode = .Favorites
            evt.accepted = true
            updateState()

        case CmdIdModeSelection:
            lastEditMode = editMode
            editMode = .Selection
            evt.accepted = true
            updateState()

        case CmdIdRestoreOrg:
            undo = capture.newUndo(.RestoreOrg)
            evt.accepted = true

        case CmdIdAlignLeft:
            undo = capture.newUndo(.AlignLeft)
            evt.accepted = true

        case CmdIdAlignCenter:
            undo = capture.newUndo(.AlignCenter)
            evt.accepted = true

        case CmdIdAlignRight:
            undo = capture.newUndo(.AlignRight)
            evt.accepted = true

        case CmdIdAlignTop:
            undo = capture.newUndo(.AlignTop)
            evt.accepted = true

        case CmdIdAlignMiddle:
            undo = capture.newUndo(.AlignMiddle)
            evt.accepted = true

        case CmdIdAlignBottom:
            undo = capture.newUndo(.AlignBottom)
            evt.accepted = true

        case CmdIdBringToFront:
            undo = capture.newUndo(.MoveToFront)
            evt.accepted = true

        case CmdIdSendToBack:
            undo = capture.newUndo(.MoveToBack)
            evt.accepted = true

        case CmdIdBringForward:
            undo = capture.newUndo(.MoveForward)
            evt.accepted = true

        case CmdIdSendBackward:
            undo = capture.newUndo(.MoveBackward)
            evt.accepted = true

        case CmdIdFlatten:
            undo = capture.newUndo(.Flatten)
            evt.accepted = true

        case CmdIdFlattenAll:
            undo = capture.newUndo(.FlattenAll)
            evt.accepted = true

        case CmdIdUndo:
            capture.undo()
            evt.accepted = true
            capture.selectionHasChanged()

        case CmdIdRedo:
            capture.redo()
            evt.accepted = true
            capture.selectionHasChanged()
        }

        if undo
            capture.pushUndo(undo)
        if evt.accepted
            invalidate()
    }
}

impl MainWnd
{
    mtd getEditMode()->EditMode
    {
        if editMode == .Selection
        {
            editMode = .Shape
            getCapture().updateEditModeFromSelection()
            edMode := editMode
            editMode = .Selection
            return edMode
        }
        elif editMode == .Favorites
        {
            #[Swag.Complete]
            switch @kindof(mdlFavorite)
            {
            case FormShape:     return EditMode.Shape
            case FormArrow:     return EditMode.Arrow
            case FormLine:      return EditMode.Line
            case FormPolygon:   return EditMode.Polygon
            case FormText:      return EditMode.Text
            case FormDraw:      return EditMode.Draw
            }
        }
        elif editMode == .Highlight
        {
            #[Swag.Complete]
            switch @kindof(mdlHighlight)
            {
            case FormShape:     return EditMode.Shape
            case FormArrow:     return EditMode.Arrow
            case FormLine:      return EditMode.Line
            case FormPolygon:   return EditMode.Polygon
            case FormText:      return EditMode.Text
            case FormDraw:      return EditMode.Draw
            }
        }

        return editMode
    }

    mtd openImage()
    {
        var opt: FileDlgOptions
        with opt
        {
            .state = &fileDlgState
            .mode = .SelectOneFile
            .title = "Load Image"
            .btnOkName = "Load"
            .shortcuts.add(@{Env.SpecialDirectory.Desktop, "Desktop"})
            .shortcuts.add(@{Env.SpecialDirectory.UserPictures, "My Images"})
            .shortcuts.add(@{Env.SpecialDirectory.CommonPictures, "Images"})
            .filters.add(@{"PNG - Portable Network Graphics (*.png)", ".png"})
            .filters.add(@{"JPG - JPEG Image (*.jpg)", ".jpg"})
            .filters.add(@{"BMP - Windows Bitmap (*.bmp)", ".bmp"})
            .filters.add(@{"TGA - Truevision Targa (*.tga)", ".tga"})
        }

        dlg := FileDlg.create(surface, opt)
        if dlg.doModal() != Dialog.BtnOk
            return

        img := catch Image.load(dlg.getSelectedName())
        if @err
            MessageDlg.error(surface, @err, "Failed to load image !")
        else
            newCapture(img)
    }

    mtd saveAsImage()
    {
        var opt: FileDlgOptions
        with opt
        {
            .state = &fileDlgState
            .mode = .CreateFile
            .title = "Save As Image"
            .btnOkName = "Save"
            .shortcuts.add(@{Env.SpecialDirectory.Desktop, "Desktop"})
            .shortcuts.add(@{Env.SpecialDirectory.UserPictures, "My Images"})
            .shortcuts.add(@{Env.SpecialDirectory.CommonPictures, "Images"})
            .editName = Path.getFileNameWithoutExtension(getCapture().file)
            .filters.add(@{"PNG - Portable Network Graphics (*.png)", ".png"})
            .filters.add(@{"JPG - JPEG Image (*.jpg)", ".jpg"})
            .filters.add(@{"BMP - Windows Bitmap (*.bmp)", ".bmp"})
            .filters.add(@{"TGA - Truevision Targa (*.tga)", ".tga"})
        }

        dlg := FileDlg.create(surface, opt)
        if dlg.doModal() != Dialog.BtnOk
            return

        Cursor.wait()
        img := getCapture().toImage()
        catch img.save(dlg.getSelectedName())
        if @err
            MessageDlg.error(surface, @err, "Failed to save file !")
    }

    mtd setBottomSize(bottomSize: BottomSize)
    {
        self.bottomSize = #self.bottomSize
        switch self.bottomSize
        {
        case Small:
            recentBar.resize(0, 80)
        case Medium:
            recentBar.resize(0, 128)
        case Big:
            recentBar.resize(0, 224)
        }

        applyLayout(@{position.width, position.height})
    }

    mtd swapLibraryMode()
    {
        libraryMode = !libraryMode

        editWnd.show(!libraryMode)
        quickStyleBar.show(!libraryMode)
        recentBar.show(!libraryMode)
        editZoomBar.show(!libraryMode)
        editToolBar.show(!libraryMode)

        libWnd.show(libraryMode)

        if libraryMode
            libWnd.fill()

        applyLayout()
    }

    mtd updateState()
    {
        captureBtn.updateCommandState()
        libraryBtn.updateCommandState()
        editToolBar.updateCommandState()
        recentSizeBar.updateCommandState()
        propWnd.selectionHasChanged()
        quickWnd.updateQuickStyles()
    }

    mtd getCapture()->*Capture
    {
        return editWnd.editView.capture
    }

    mtd newImage()
    {
        dlg := Dialog.createDialog(surface, 350, 235, title: "New Image")
        dlg.addButton("Create", Dialog.BtnOk, true)
        dlg.addButton("Cancel", Dialog.BtnCancel)
        edit0 := EditCtrl.create(dlg, "Width", "512", @{90, 12}, flags: .RightAlign)
        edit1 := EditCtrl.create(dlg, "Height", "512", @{90, 47}, flags: .RightAlign)
        edit0.edit.setInputU64(16, 4096)
        edit1.edit.setInputU64(16, 4096)
        if dlg.doModal() != Dialog.BtnOk
            return

        (w, ?, ?) := catch StrConv.parseS32(edit0.edit.text)
        (h, ?, ?) := catch StrConv.parseS32(edit1.edit.text)
        img := Image.create(w orelse 512, h orelse 512, .RGB8)
        img.fill(Argb.White)
        newCapture(img)
    }

    mtd newCapture(img: Image)
    {
        if !img.isValid()
            return

        with capture := Memory.new'Capture()
        {
            .changed = true
            .width, .widthOrg = img.width
            .height, .heightOrg = img.height
            .backImg, .backImgOrg = img
            .file = Library.newFileName()
        }

        editWnd.editView.setCapture(capture)
        editWnd.editView.zoomToFit(false)

        capture.computePreview()
        recentWnd.addRecent(capture.file, capture.preview)
        recentWnd.recentView.selected = 0
        catch capture.save()
        g_Library.addFile(capture.file)

        // Force edition mode
        if libraryMode
        {
            swapLibraryMode()
            updateState()
        }
    }

    mtd captureScreen(mode: CaptureMode)
    {
        if hideCaptMe
        {
            if !surface.isMinimized
                surface.hide()
        }
        else
            surface.disable()

        var img: Image
        if mode == .None
            img = catch screenShot(captureMode)
        else
            img = catch screenShot(mode)

        if hideCaptMe
        {
            if !surface.isMinimized
                surface.show()
            else
                surface.showNormal()
        }
        else
            surface.enable()

        if @err or img.width == 0 or img.height == 0
            return
        newCapture(img)
    }

    mtd updateZoomDisplay()
    {
        z := cast(s32) (editWnd.editView.zoom * 100)
        zoomBtn.name = Format.toString("% %%", z)
        zoomBtn.invalidate()
    }

    mtd registerShortcuts()
    {
        registerKeyShortcut(.Control,          .C,              CmdIdCopy)
        registerKeyShortcut(.Control,          .V,              CmdIdPaste)
        registerKeyShortcut(.Control,          .Insert,         CmdIdCopy)
        registerKeyShortcut(.Shift,            .Insert,         CmdIdPaste)
        registerKeyShortcut(.Control,          .Z,              CmdIdUndo)
        registerKeyShortcut(.Control,          .Y,              CmdIdRedo)
        registerKeyShortcut(.CtrlShift,        .F,              CmdIdBringForward)
        registerKeyShortcut(.CtrlShift,        .B,              CmdIdSendBackward)
        registerKeyShortcut(.None,             .Delete,         CmdIdDeleteSelection)
        registerKeyShortcut(.CtrlShift,        .A,              CmdIdCopyAsImage)
        registerKeyShortcut(.Control,          .A,              CmdIdSelectAll)
        registerKeyShortcut(.Control,          .D,              CmdIdDeSelectAll)
        registerKeyShortcut(.Control,          .Add,            CmdIdZoomIn)
        registerKeyShortcut(.Control,          .Subtract,       CmdIdZoomOut)
        registerKeyShortcut(.Control,          .NumPad0,        CmdIdZoom1)
        registerKeyShortcut(.CtrlShift,        .D,              CmdIdDupCapture)
        registerKeyShortcut(.CtrlShift,        .I,              CmdIdInvertSelection)
        registerKeyShortcut(.CtrlShift,        .T,              CmdIdFlattenAll)
        registerKeyShortcut(.Alt,              .F4,             CmdIdExit)
        registerKeyShortcut(.Control,          .N,              CmdIdNewImage)
        registerKeyShortcut(.CtrlShift,        .N,              CmdIdNewFromClipboard)
        registerKeyShortcut(.CtrlShift,        .S,              CmdIdSaveAsImage)
        registerKeyShortcut(.Control,          .O,              CmdIdOpenImage)
        registerKeyShortcut(.Shift,            .Left,           CmdIdMoveSelLeft1)
        registerKeyShortcut(.Shift,            .Up,             CmdIdMoveSelTop1)
        registerKeyShortcut(.Shift,            .Right,          CmdIdMoveSelRight1)
        registerKeyShortcut(.Shift,            .Down,           CmdIdMoveSelBottom1)
        registerKeyShortcut(.None,             .Left,           CmdIdMoveSelLeft)
        registerKeyShortcut(.None,             .Up,             CmdIdMoveSelTop)
        registerKeyShortcut(.None,             .Right,          CmdIdMoveSelRight)
        registerKeyShortcut(.None,             .Down,           CmdIdMoveSelBottom)

        errorReg := false
        #[Swag.Mixin]
        func registerHotKey(mdf: Input.KeyModifiers, key: Input.Key, wndId: WndId)
        {
            catch getApp().registerHotKey(mdf, key, wndId, self)
            if @err
            {
                if !errorReg
                {
                    msg := Format.toString("Cannot register global shortcut '%'. ", Input.getKeyName(mdf, key))
                    msg += "Another running application is probably already using it !"
                    discard EmbInfoCtrl.create(self.editWnd, msg, .Critical)
                }

                errorReg = true
            }
        }

        registerHotKey(.None,      .PrintScreen, CmdIdCapture)
        registerHotKey(.Control,   .PrintScreen, CmdIdCaptureFullScreen)
        registerHotKey(.CtrlShift, .PrintScreen, CmdIdCaptureArea)
        registerHotKey(.Alt,       .PrintScreen, CmdIdCaptureActiveWnd)
    }

    mtd createTopMenu()
    {
        with topMenu := MenuCtrl.createBar(parent, owner: self)
        {
            .bringToFront()
            .dockStyle = .Top
            .margin.y = -parent.padding.y
            .backgroundStyle = .Transparent
        }

        with fileMenu := MenuCtrl.createPopup(parent, owner: self)
        {
            .addItem(CmdIdNewImage)
            .addItem(CmdIdNewFromClipboard)
            .addSeparator()
            .addItem(CmdIdOpenImage)
            .addItem(CmdIdSaveAsImage)
            .addSeparator()
            .addItem(CmdIdExit)
            topMenu.addPopup("File", fileMenu)
        }

        with captureMenu := MenuCtrl.createPopup(parent, owner: self)
        {
            .addItem(CmdIdCaptModeFullScreen)
            .addItem(CmdIdCaptModeArea)
            .addItem(CmdIdCaptModeLastArea)
            .addSeparator()
            .addItem(CmdIdCapture)
            .addSeparator()
            .addItem(CmdIdCaptureArea)
            .addItem(CmdIdCaptureFullScreen)
            .addItem(CmdIdCaptureActiveWnd)

            visit m, idx: monitors
            {
                switch idx
                {
                case 0: .addItem(CmdIdCaptureScreen0)
                case 1: .addItem(CmdIdCaptureScreen1)
                case 2: .addItem(CmdIdCaptureScreen2)
                case 3: .addItem(CmdIdCaptureScreen3)
                }
            }

            .addSeparator()
            .addItem(CmdIdCaptureHideEditor)

            topMenu.addPopup("Capture", captureMenu)
        }

        with editMenu := MenuCtrl.createPopup(parent, owner: self)
        {
            .addItem(CmdIdUndo)
            .addItem(CmdIdRedo)
            .addItem(CmdIdRestoreOrg)
            .addSeparator()
            .addItem(CmdIdCopy)
            .addItem(CmdIdPaste)
            .addItem(CmdIdCopyAsImage)
            .addSeparator()
            .addItem(CmdIdSelectAll)
            .addItem(CmdIdDeSelectAll)
            .addItem(CmdIdInvertSelection)
            topMenu.addPopup("Edit", editMenu)
        }

        with viewMenu := MenuCtrl.createPopup(parent, owner: self)
        {
            .addItem(CmdIdZoomIn)
            .addItem(CmdIdZoomOut)
            .addItem(CmdIdZoom1)
            .addItem(CmdIdZoomFit)
            topMenu.addPopup("View", viewMenu)
        }
    }

    mtd createToolbar(topBar: *Wnd)
    {
        mtd createBtn(id: WndId)
        {
            with b := IconButton.create(editToolBar, position: @{0, 0, 60, 64})
            {
                .id = #self.id
                .style.addStyleSheetColors("btnIcon_CheckedBk    $btnIcon_HotBk")
                .style.addStyleSheetColors("btnIcon_CheckedHotBk $btnIcon_HotBk")
                .iconBtnFlags |= .Checkable
                .checkedForm = .Bottom
            }
        }

        with editToolBar = StackLayoutCtrl.create(topBar, .HorzCenter, @{0, 0, 64, 64})
        {
            .dockStyle = .Top
            .backgroundStyle = .Window
            .style.addStyleSheetColors("wnd_Bk $wnd_Caption")
        }

        createBtn(CmdIdModeFavorites)
        createBtn(CmdIdModeSelection)
        createBtn(CmdIdModeShape)
        createBtn(CmdIdModeArrow)
        createBtn(CmdIdModeLine)
        createBtn(CmdIdModePolygon)
        createBtn(CmdIdModeText)
        createBtn(CmdIdModeDraw)
        createBtn(CmdIdModeHighlight)
        editToolBar.updateCommandState()
    }

    func create(parent: *Wnd)->*MainWnd
    {
        parent.surface.minSize = @{1024, 512}

        img24 := assume Image.decode(".png", Icons24)
        img48 := assume Image.decode(".png", Icons48)
        renderer := parent.getApp().getRenderer()

        with res := Wnd.create'MainWnd(parent, "captme.mainwnd")
        {
            .icons24.set(renderer.addImage(img24), 24, 24)
            .icons48.set(renderer.addImage(img48), 48, 48)
            .clipFmt = catch Clipboard.registerFormat(ClipboardFormat)
            .backgroundStyle = .Window
            .mdlFavorite  = &res.mdlFormShape
            .mdlHighlight = &res.mdlFormShape
            .monitors = Env.getMonitors()
        }

        with topBar := Wnd.create'Wnd(res, @{0, 0, 64, 68})
        {
            .dockStyle = .Top
            .backgroundStyle = .Window
            .style.addStyleSheetColors("wnd_Bk $wnd_Caption")
        }

        res.createToolbar(topBar)

        res.captureBtn = IconButton.create(topBar, Icon.from(&res.icons48, 6), @{0, 0, 64, 64}, id: CmdIdCapture)
        res.libraryBtn = IconButton.create(topBar, Icon.from(&res.icons48, 8), @{64, 0, 64, 64}, id: CmdIdLibrary)

        // Main edit area
        with res.editWnd = EditWnd.create(res, res)
        {
            .dockStyle = .Center
            .sigScrollSizeChanged += @|res|(scroll, old, new) {
                res.updateZoomDisplay()
            }
        }

        with res.recentBar = Wnd.create'Wnd(res, @{0, 0, 0, 128})
        {
            .dockStyle = .Bottom
        }

        // Recent left editToolBar
        with res.recentSizeBar = IconBar.create(res.recentBar, 20)
        {
            .setIconBarFlags(.Vertical)
            .dockStyle = .Left
            .iconPadding = 0
            .addItem(Icon.from(&res.icons24, 0), CmdIdRecentSmall, .Checkable)
            .addItem(Icon.from(&res.icons24, 1), CmdIdRecentMedium, .Checkable)
            .addItem(Icon.from(&res.icons24, 2), CmdIdRecentBig, .Checkable)
        }

        // Recent
        with res.recentWnd = RecentWnd.create(res.recentBar, res)
        {
            .dockStyle = .Center
        }

        // Right bar
        with res.quickStyleBar = SplitterCtrl.create(res, @{0, 0, 300, 600}, vertical: true)
        {
            .dockStyle = .Right
            .splitterFlags |= .HideBars
            .backgroundStyle = .Window
        }

        with res.quickWnd = QuickWnd.create(res.quickStyleBar)
        {
            .main = res
        }

        with res.propWnd = PropWnd.create(res.quickStyleBar)
        {
            .main = res
        }

        item := res.quickStyleBar.addPane(res.quickWnd, title: true)
        item.tryPreserveSize = true
        item.label.setText("Quick Styles")
        item = res.quickStyleBar.addPane(res.propWnd, title: true)
        item.label.setText("Tool Properties")

        res.quickStyleBar.setPaneSize(0, 300)

        // Bottom bar
        with res.editZoomBar = StackLayoutCtrl.create(res, .HorzCenter, @{0, 0, 300, 32})
        {
            .dockStyle = .Bottom
            .backgroundStyle = .Window
        }

        with res.zoomBtn = IconButton.create(res.editZoomBar, res.getTheme().getIcon24(ThemeIcons24.Search), @{0, 0, 110, 32})
        {
            .textPos = .Right
            .arrowPos = .RightUp
            .sigPressed += @|res|(btn) {
                pos  := btn.getSurfaceRect()
                menu := MenuCtrl.createPopup(res)
                menu.menuCtrlFlags |= .NoKeyShortcuts
                menu.addItem(CmdIdZoom1)
                menu.addItem(CmdIdZoomFit)
                discard menu.doModal(@{pos.horzCenter(), pos.y}, .AnchorBottomCenter, autoDestroy: true)
            }
        }

        with res
        {
            .registerShortcuts()
            .updateZoomDisplay()
            .setBottomSize(.Medium)
            .updateState()
            .libWnd = LibraryWnd.create(res)
            .libWnd.hide()
        }

        return res
    }
}