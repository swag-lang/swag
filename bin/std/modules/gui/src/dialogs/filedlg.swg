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
        if me.dlgOptions.state
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
        if me.comboPathStr.count > 1:
            me.setCurFolder(me.comboPathStr[1].toString())
    }

    mtd setCurFolder(folder: string)
    {
        if !@countof(folder) or !Directory.exists(folder):
            return

        me.curFolder = folder
        Path.normalize(&me.curFolder)

        if me.curFolder.back() == '/':
            me.curFolder.removeBack()

        me.fillFileList()
        me.fillComboPath()
    }

    mtd fillComboPath()
    {
        me.comboPath.clear()
        me.comboPathStr.clear()

        let all = Tokenize.split(me.curFolder.toString(), '/''rune)
        for #reverse i in all
        {
            var str: String
            for j in i + 1
            {
                str += all[j]
                str += '/''u8
            }

            me.comboPathStr.add(str)

            let one  = all[i]
            var icon = me.getApp().getDirectoryIcon(one)
            icon.color = Pixel.Argb.White
            me.comboPath.addItem(one, icon: icon)
        }

        me.comboPath.selectItem(0)
    }

    mtd fillFileListFolders()
    {
        var l = me.treeCtrl.createLine("Shortcuts")
        l.separator = true

        foreach v in me.dlgOptions.shortcuts
        {
            let c    = Env.getSpecialDirectory(v.item0)
            var icon = me.getApp().getSpecialDirectoryIcon(v.item0, true)
            icon.color       = Pixel.Argb.White
            l                = me.treeCtrl.createLine(v.item1, icon)
            l.userData0      = cast(*void) me.treePathStr.count
            l.leftTextMargin = 10
            me.treePathStr.add(c)
        }

        l           = me.treeCtrl.createLine("Drives")
        l.separator = true

        let volumes = catch Directory.getDrives()
        foreach v in volumes
        {
            v.removeBack()
            var icon = me.getApp().getDirectoryIcon(v, true)
            icon.color       = Pixel.Argb.White
            l                = me.treeCtrl.createLine(v.toString(), icon)
            l.userData0      = cast(*void) me.treePathStr.count
            l.leftTextMargin = 10
            me.treePathStr.add(v)
        }

        me.treeCtrl.sigSelChanged += mtd||(list)
        {
            var line = list.getSelectedLine()
            if !line:
                return
            var idx = cast(u64) line.userData0
            me.setCurFolder(me.treePathStr[idx].toString())
        }
    }

    mtd fillFileList()
    {
        me.listCtrl.clear()

        with var options: EnumerationOptions
        {
            .skipAttributes  = .Hidden
            .wantFiles       = me.dlgOptions.mode != .SelectFolder
            .wantDirectories = true
            .recurse         = false
        }

        me.curList = catch Directory.enumerate(me.curFolder, options)

        let item   = me.comboType.getSelectedItem()
        let filter = me.dlgOptions.filters[cast(u64) item.userData0]
        let tokens = Tokenize.split(filter.item1, ';''rune)

        foreach &fi in me.curList
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

            if !show:
                continue

            // Content
            var icon = me.getApp().getFileDirectoryIcon(fname, fi.isDirectory())
            icon.color = Pixel.Argb.White
            let line = me.listCtrl.createLine(fname, icon)
            line.userData0 = fi
            line.setItem(1, fi.lastWriteTime.toString(.DateTimeIso))
        }

        me.listCtrl.computeLayout()
        me.listCtrl.sort()
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
                    me.folderUp()

                case Enter:
                    let l = list.getFocusLine()
                    if !l:
                        break
                    let fi = cast(const *FileInfo) l.userData0
                    if fi.isDirectory()
                    {
                        me.setCurFolder(fi.fullname)
                    }
                    elif me.dlgOptions.mode != .SelectFolder
                    {
                        me.validateId(Dialog.BtnOk)
                        list.setFocus()
                    }
                }
            }
            .sigLeftDoubleClick += mtd||(list, pt)
            {
                var l = list.getSelectedLine()
                if !l:
                    return
                var fi = cast(const *FileInfo) l.userData0
                if fi.isDirectory()
                {
                    me.setCurFolder(fi.fullname)
                }
                elif me.dlgOptions.mode != .SelectFolder
                {
                    me.validateId(Dialog.BtnOk)
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
                        if me.dlgOptions.mode != .SelectFolder:
                            continue
                    }
                    else
                    {
                        if me.dlgOptions.mode == .SelectFolder:
                            continue
                    }

                    if !txt.isEmpty():
                        txt += ";"
                    txt += Path.getFileName(fi.fullname)
                }

                if !txt.isEmpty():
                    me.editFile.setText(txt)
            }
            .sigSort = func(l1, l2, col, down)
            {
                var fi1 = cast(*FileInfo) l1.userData0
                var fi2 = cast(*FileInfo) l2.userData0
                if fi1.isDirectory() and !fi2.isDirectory():
                    return -1
                if !fi1.isDirectory() and fi2.isDirectory():
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
            if btnid != Dialog.BtnOk:
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

            var path = Path.combine(me.curFolder, folderName.toString())
            if Directory.exists(path)
            {
                MessageDlg.error(dlg.surface, "Folder already exists")
                dlg.edit.selectAll()
                dlg.edit.setFocus()
                return false
            }

            return true
        }

        if dlg.doModal() != Dialog.BtnOk:
            return

        let path = Path.combine(me.curFolder, dlg.edit.text.toString())
        catch Directory.create(path)
        if @err != null
        {
            MessageDlg.error(me.surface, "Cannot create folder")
            return
        }

        me.fillFileList()
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

            if opt.title:
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
            if opt.mode == .SelectFolder:
                .addItem("Folders")
            else:
                foreach f, idx in opt.filters:
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
            if opt.openFolder:
                .setCurFolder(opt.openFolder)
            elif opt.state:
                .setCurFolder(opt.state.curFolder)
            .listCtrl.sort(0)
            .editFile.setText(opt.editName)
            if opt.mode == .CreateFile
            {
                .editFile.selectAll()
                .editFile.setFocus()
            }
            else:
                .listCtrl.setFocus()
        }

        // Take care of state
        if opt.state
        {
            with opt.state
            {
                if !.position.isEmpty():
                    dlg.surface.setPosition(.position)
                else:
                    dlg.surface.center(from)

                if !.curFolder.isEmpty():
                    dlg.setCurFolder(.curFolder)
                else:
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
        let sel = me.getSelectedNames()
        if sel.count == 0:
            return String.from("")
        return sel.front()
    }

    // Get all selections
    mtd getSelectedNames()->Array'String
    {
        var result: retval

        let item = me.comboType.getSelectedItem()
        let fi   = me.dlgOptions.filters[cast(u64) item.userData0]

        let tokens = Tokenize.split(me.editFile.text, ';''rune)
        foreach t in tokens
        {
            // Be sure this is a valid file/folder name
            var t1 = String.from(t)
            t1.trim()
            if t1.isEmpty():
                continue
            if me.dlgOptions.mode == .SelectFolder and !Path.isValidPathName(t1.toString()):
                continue
            if me.dlgOptions.mode != .SelectFolder and !Path.isValidFileName(t1.toString()):
                continue

            // Get the full name
            var val = Path.combine(me.curFolder, t1.toString())

            // Append extension if not done by the user
            if me.dlgOptions.mode != .SelectFolder and !Path.isExtension(val, fi.item1)
            {
                let exts = Tokenize.split(fi.item1, ';''rune)
                val += exts[0]
            }

            result.add(val)
        }

        return result
    }
}
