#global public
using Core, File, Directory

enum FileDlgMode
{
    CreateFile
    SelectOneFile
    SelectMultiFiles
    SelectFolder
}

struct FileDlgState
{
    position:                Math.Rectangle
    curFolder:               String
    folderHistory:           Array'String
    columnWidthName:         s32 = 200
    columnWidthModified:     s32 = 150
}

struct FileDlgOptions
{
    title:          string
    btnOkName:      string
    openFolder:     string
    editName:       string
    drivePane       = true
    mode:           FileDlgMode = SelectOneFile
    filters:        Array'{ name: string, extensions: string }
    shortcuts:      Array'{ type: Core.Env.SpecialDirectory, name: string }
    state:          *FileDlgState
}

struct FileDlg
{
    using dialog:     Dialog

    treeCtrl:         *ListCtrl
    treePathStr:      Array'String
    listCtrl:         *ListCtrl
    comboPath:        *ComboBox
    comboPathStr:     Array'String
    editFile:         *EditBox
    comboType:        *ComboBox
    dlgOptions:       FileDlgOptions

    curFolder:        String
    curList:          Array'FileInfo
}

impl IWnd for FileDlg
{
    mtd impl onDestroyEvent(evt: *DestroyEvent)
    {
        if .dlgOptions.state
        {
            with me.dlgOptions.state
            {
                .position            = me.surface.position
                .curFolder           = me.curFolder
                .columnWidthName     = cast() me.listCtrl.header.getItemWidthByCol(0)
                .columnWidthModified = cast() me.listCtrl.header.getItemWidthByCol(1)
            }
        }
    }
}

internal impl FileDlg
{
    mtd folderUp()
    {
        if .comboPathStr.count > 1 do
            .setCurFolder(.comboPathStr[1].toString())
    }

    mtd setCurFolder(folder: string)
    {
        if !@countof(folder) or !Directory.exists(folder) do
            return

        .curFolder = folder
        Path.normalize(&.curFolder)

        if .curFolder.back() == '/' do
            .curFolder.removeBack()

        .fillFileList()
        .fillComboPath()
    }

    mtd fillComboPath()
    {
        .comboPath.clear()
        .comboPathStr.clear()

        let all = Tokenize.split(.curFolder.toString(), '/''rune)
        for #reverse i in all
        {
            var str: String
            for j in i + 1
            {
                str += all[j]
                str += '/''u8
            }

            .comboPathStr.add(str)

            let one  = all[i]
            var icon = .getApp().getDirectoryIcon(one)
            icon.color = Pixel.Argb.White
            .comboPath.addItem(one, icon: icon)
        }

        .comboPath.selectItem(0)
    }

    mtd fillFileListFolders()
    {
        var l = .treeCtrl.createLine("Shortcuts")
        l.separator = true

        foreach v in .dlgOptions.shortcuts
        {
            let c    = Env.getSpecialDirectory(v.item0)
            var icon = .getApp().getSpecialDirectoryIcon(v.item0, true)
            icon.color       = Pixel.Argb.White
            l                = .treeCtrl.createLine(v.item1, icon)
            l.userData0      = cast(*void) .treePathStr.count
            l.leftTextMargin = 10
            .treePathStr.add(c)
        }

        l           = .treeCtrl.createLine("Drives")
        l.separator = true

        let volumes = catch Directory.getDrives()
        foreach v in volumes
        {
            v.removeBack()
            var icon = .getApp().getDirectoryIcon(v, true)
            icon.color       = Pixel.Argb.White
            l                = .treeCtrl.createLine(v.toString(), icon)
            l.userData0      = cast(*void) .treePathStr.count
            l.leftTextMargin = 10
            .treePathStr.add(v)
        }

        .treeCtrl.sigSelChanged += mtd||(list)
        {
            var line = list.getSelectedLine()
            if !line do
                return
            var idx = cast(u64) line.userData0
            .setCurFolder(.treePathStr[idx].toString())
        }
    }

    mtd fillFileList()
    {
        .listCtrl.clear()

        with var options: EnumerationOptions
        {
            .skipAttributes  = .Hidden
            .wantFiles       = me.dlgOptions.mode != .SelectFolder
            .wantDirectories = true
            .recurse         = false
        }

        .curList = catch Directory.enumerate(.curFolder, options)

        let item   = .comboType.getSelectedItem()
        let filter = .dlgOptions.filters[cast(u64) item.userData0]
        let tokens = Tokenize.split(filter.item1, ';''rune)

        foreach &fi in .curList
        {
            let fname = assume Path.getFileName(fi.fullname)

            // Filter
            var show = true
            if !fi.isDirectory() and @countof(filter.item1)
            {
                show = false
                foreach t in tokens
                {
                    if Path.isExtension(fname, t)
                    {
                        show = true
                        break
                    }
                }
            }

            if !show do
                continue

            // Content
            var icon = .getApp().getFileDirectoryIcon(fname, fi.isDirectory())
            icon.color = Pixel.Argb.White
            let line = .listCtrl.createLine(fname, icon)
            line.userData0 = fi
            line.setItem(1, fi.lastWriteTime.toString(.DateTimeIso))
        }

        .listCtrl.computeLayout()
        .listCtrl.sort()
    }

    mtd createFileList(view: *Wnd)
    {
        with me.listCtrl = ListCtrl.createMultiColumns(view)
        {
            .selectionMode = me.dlgOptions.mode == .SelectMultiFiles ? .Multi : .Single
            .dockStyle     = .Center
            .margin.z      = 10
            .listFlags |= .HotTrack
            .backgroundStyle = .View
            .addColumn("Name", me.dlgOptions.state ? me.dlgOptions.state.columnWidthName : 200)
            .addColumn("Modified", me.dlgOptions.state ? me.dlgOptions.state.columnWidthModified : 150)
            .header.form = .Transparent
            .header.style.addStyleSheetColors("headerItem_Separator $lighterMedium")
            .scrollWnd.style.addStyleSheetColors("scrollBar_Bk $view_Bk")
            .sigKeyPressed += mtd||(list, evt)
            {
                switch evt.key
                {
                case Back:
                    .folderUp()

                case Enter:
                    let l = list.getFocusLine()
                    if !l do
                        break
                    let fi = cast(const *FileInfo) l.userData0
                    if fi.isDirectory()
                    {
                        .setCurFolder(fi.fullname)
                    }
                    elif .dlgOptions.mode != .SelectFolder
                    {
                        .validateId(Dialog.BtnOk)
                        list.setFocus()
                    }
                }
            }
            .sigLeftDoubleClick += mtd||(list, pt)
            {
                var l = list.getSelectedLine()
                if !l do
                    return
                var fi = cast(const *FileInfo) l.userData0
                if fi.isDirectory()
                {
                    .setCurFolder(fi.fullname)
                }
                elif .dlgOptions.mode != .SelectFolder
                {
                    .validateId(Dialog.BtnOk)
                    list.setFocus()
                }
            }
            .sigSelChanged += mtd||(list)
            {
                var sel  = list.getSelectedLines()
                var txt: String
                foreach l in sel
                {
                    let fi = cast(const *FileInfo) l.userData0
                    if fi.isDirectory()
                    {
                        if .dlgOptions.mode != .SelectFolder do
                            continue
                    }
                    else
                    {
                        if .dlgOptions.mode == .SelectFolder do
                            continue
                    }

                    if !txt.isEmpty() do
                        txt += ";"
                    txt += Path.getFileName(fi.fullname)
                }

                if !txt.isEmpty() do
                    .editFile.setText(txt)
            }
            .sigSort = func(l1, l2, col, down)
            {
                var fi1 = cast(*FileInfo) l1.userData0
                var fi2 = cast(*FileInfo) l2.userData0
                if fi1.isDirectory() and !fi2.isDirectory() do
                    return -1
                if !fi1.isDirectory() and fi2.isDirectory() do
                    return 1
                switch col
                {
                case 0:
                    return (fi1.fullname <=> fi2.fullname) * (down ? -1 : 1)
                case 1:
                    return (fi1.lastWriteTime <=> fi2.lastWriteTime) * (down ? -1 : 1)
                }

                return 0
            }
        }
    }

    mtd onNewFolder()
    {
        let folderName: String = "New Folder"

        with let dlg = EditDlg.createSingleLine(me.surface)
        {
            .edit.setText(folderName)
            .edit.selectAll()
            .edit.setNote("Folder name")
            .surface.setTitle("Folder Name")
            .surface.center(me.surface)
        }

        dlg.sigPressedButton = mtd|dlg|(fdlg, btnid)
        {
            if btnid != Dialog.BtnOk do
                return true

            var folderName = dlg.edit.text
            folderName.trim()
            if folderName.isEmpty()
            {
                dlg.edit.selectAll()
                dlg.edit.setFocus()
                return false
            }

            if !Path.isValidFileName(folderName)
            {
                MessageDlg.error(dlg.surface, "Invalid folder name")
                dlg.edit.selectAll()
                dlg.edit.setFocus()
                return false
            }

            var path = Path.combine(.curFolder, folderName.toString())
            if Directory.exists(path)
            {
                MessageDlg.error(dlg.surface, "Folder already exists")
                dlg.edit.selectAll()
                dlg.edit.setFocus()
                return false
            }

            return true
        }

        if dlg.doModal() != Dialog.BtnOk do
            return

        let path = Path.combine(.curFolder, dlg.edit.text.toString())
        catch Directory.create(path)
        if @err != null
        {
            MessageDlg.error(.surface, "Cannot create folder")
            return
        }

        .fillFileList()
    }
}

impl FileDlg
{
    // Creates the message box
    func create(from: *Surface, opt: FileDlgOptions = {})->*FileDlg
    {
        let dlg = Wnd.create'FileDlg()
        dlg.fromSurface = from

        with let srf = dlg.createSurface(from, 100, 100, 700, 500)
        {
            .setSurfaceFlags(add: .Sizeable)
            .minSize = {400, 400}

            if opt.title do
                .setTitle(opt.title)
            else
            {
                #[Swag.Complete]
                switch opt.mode
                {
                case CreateFile:
                    .setTitle("Create File")
                case SelectOneFile:
                    .setTitle("Select File")
                case SelectMultiFiles:
                    .setTitle("Select Files")
                case SelectFolder:
                    .setTitle("Select Folder")
                }
            }
        }

        dlg.dlgOptions = opt
        dlg.addButton(opt.btnOkName orelse "Ok", Dialog.BtnOk, true)
        dlg.addButton("Cancel", Dialog.BtnCancel)
        dlg.backgroundStyle = .Window
        srf.setSurfaceFlags(remove: .CloseBtn)

        dlg.sigPressedButton = func|dlg|(bdlg, id)
        {
            if id == Dialog.BtnOk
            {
                let selName = dlg.getSelectedName()
                if selName == null
                {
                    dlg.editFile.selectAll()
                    dlg.editFile.setFocus()
                    return false
                }

                if dlg.dlgOptions.mode == .CreateFile and File.exists(selName)
                {
                    let res = MessageDlg.confirm(dlg.surface, "This file already exists.", "Do you want to replace it ?")
                    return res == Dialog.BtnYes
                }
            }

            return true
        }

        let view = Wnd.create'Wnd(dlg)
        view.dockStyle = .Center

        // Panes
        ////////////////////////

        if opt.drivePane
        {
            with dlg.treeCtrl = ListCtrl.createSimple(view, {0, 0, 170})
            {
                .dockStyle = .Left
                .listFlags |= .HotTrack
                .lineHeight      = 28
                .leftTextMargin  = 8
                .backgroundStyle = .Dialog
                .scrollWnd.style.addStyleSheetColors("scrollBar_Bk $view_Bk")
                dlg.fillFileListFolders()
            }
        }

        dlg.createFileList(view)

        // Top
        ////////////////////////
        with let viewTop = Wnd.create'Wnd(view, {0, 0, 0, 50})
        {
            .dockStyle       = .Top
            .backgroundStyle = .Window
            .padding.x       = 10
            .padding.z       = 10
            .padding.y       = 8
            .padding.w       = 8
        }

        dlg.applyLayout()

        if opt.mode == .CreateFile or opt.mode == .SelectFolder
        {
            let btn = PushButton.create(viewTop, "New Folder", {0, 0, 90})
            with btn
            {
                .dockStyle = .Right
                .margin.x  = 10
                .sigPressed += func|dlg|(btn)
                {
                    dlg.onNewFolder()
                }
            }
        }

        with dlg.comboPath = ComboBox.create(viewTop)
        {
            .dockStyle = .Center
            .sigChanged += func|dlg|(combo, idx)
            {
                dlg.setCurFolder(dlg.comboPathStr[idx])
            }
        }

        // Bottom
        ////////////////////////
        let viewBottom = Wnd.create'Wnd(view, {0, 0, 0, 90})
        viewBottom.dockStyle       = .Bottom
        viewBottom.backgroundStyle = .Window
        dlg.applyLayout()

        with dlg.editFile = EditBox.create(viewBottom, "", {105, 10, viewBottom.position.width - (105 + 15)})
        {
            .anchorStyle = .Width
        }

        with dlg.comboType = ComboBox.create(viewBottom, {105, 45, viewBottom.position.width - (105 + 15)})
        {
            .anchorStyle = .Width
            if opt.mode == .SelectFolder do
                .addItem("Folders")
            else do
                foreach f, idx in opt.filters do
                    .addItem(f.item0, userData0: cast(*void) idx)
            .selectItem(0)
            .sigChanged += func|dlg|(c, idx)
            {
                dlg.fillFileList()
            }
        }

        let nameLbl0 = opt.mode == .SelectFolder ? "Folder Name" : "File Name"
        let label0   = Label.create(viewBottom, nameLbl0, {5, 10, 80, dlg.editFile.position.height})
        label0.horzAlignmentText = .Right
        label0.vertAlignmentText = .Center
        let label1 = Label.create(viewBottom, "Type", {5, 45, 80, dlg.comboType.position.height})
        label1.horzAlignmentText = .Right
        label1.vertAlignmentText = .Center

        with dlg
        {
            if opt.openFolder do
                .setCurFolder(opt.openFolder)
            elif opt.state do
                .setCurFolder(opt.state.curFolder)
            .listCtrl.sort(0)
            .editFile.setText(opt.editName)
            if opt.mode == .CreateFile
            {
                .editFile.selectAll()
                .editFile.setFocus()
            }
            else do
                .listCtrl.setFocus()
        }

        // Take care of state
        if opt.state
        {
            with opt.state
            {
                if !.position.isEmpty() do
                    dlg.surface.setPosition(.position)
                else do
                    dlg.surface.center(from)

                if !.curFolder.isEmpty() do
                    dlg.setCurFolder(.curFolder)
                else do
                    dlg.treeCtrl.selectLine(0)
            }
        }
        else
        {
            dlg.surface.center(from)
            dlg.treeCtrl.selectLine(0)
        }

        return dlg
    }

    // Get the first selection
    mtd getSelectedName()->String
    {
        let sel = .getSelectedNames()
        if sel.count == 0 do
            return String.from("")
        return sel.front()
    }

    // Get all selections
    mtd getSelectedNames()->Array'String
    {
        var result: retval

        let item = .comboType.getSelectedItem()
        let fi   = .dlgOptions.filters[cast(u64) item.userData0]

        let tokens = Tokenize.split(.editFile.text, ';''rune)
        foreach t in tokens
        {
            // Be sure this is a valid file/folder name
            var t1 = String.from(t)
            t1.trim()
            if t1.isEmpty() do
                continue
            if .dlgOptions.mode == .SelectFolder and !Path.isValidPathName(t1.toString()) do
                continue
            if .dlgOptions.mode != .SelectFolder and !Path.isValidFileName(t1.toString()) do
                continue

            // Get the full name
            var val = Path.combine(.curFolder, t1.toString())

            // Append extension if not done by the user
            if .dlgOptions.mode != .SelectFolder and !Path.isExtension(val, fi.item1)
            {
                let exts = Tokenize.split(fi.item1, ';''rune)
                val += exts[0]
            }

            result.add(val)
        }

        return result
    }
}
