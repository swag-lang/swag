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
    position:   Math.Rectangle
    curFolder:  String
}

struct FileDlgOptions
{
    title:      string
    btnOkName:  string
    openFolder: string
    editName:   string
    drivePane   = true
    mode:       FileDlgMode = SelectOneFile
    filters:    Array'{name: string, extensions: string}
    shortcuts:  Array'{type: Env.SpecialDirectory, name: string}
    state:      *FileDlgState
}

struct FileDlg
{
    using dialog: Dialog

    treeCtrl:       *ListCtrl
    treePathStr:    Array'String
    listCtrl:       *ListCtrl
    comboPath:      *ComboBox
    comboPathStr:   Array'String
    editFile:       *EditBox
    comboType:      *ComboBox
    dlgOptions:     FileDlgOptions

    curFolder:      String
    curList:        Array'FileInfo
}

impl IWnd for FileDlg
{
    mtd impl onDestroyEvent(evt: *DestroyEvent)
    {
        if evt.kind == .Destroy and dlgOptions.state
        {
            with dlgOptions.state
            {
                .position = surface.position
                .curFolder = curFolder
            }
        }
    }
}

private impl FileDlg
{
    mtd folderUp()
    {
        if comboPathStr.count > 1
            setCurFolder(comboPathStr[1].toString())
    }

    mtd setCurFolder(folder: string)
    {
        if !@countof(folder) or !Directory.exists(folder)
            return

        curFolder = folder
        Path.normalize(&curFolder)

        if curFolder.back() == "/"'u8
            curFolder.removeBack()

        fillList()
        fillComboPath()
    }

    mtd fillComboPath()
    {
        comboPath.clear()
        comboPathStr.clear()

        var all = Tokenize.split(curFolder.toString(), "/"'rune)
        loop i: all.count-1..0
        {
            var str: String
            loop j: i+1
            {
                str += all[j]
                str += "/"'u8
            }

            comboPathStr.add(str)

            let one = all[i]
            var ic = getApp().getDirectoryIcon(one)
            comboPath.addItem(one, icon = ic)
        }

        comboPath.selectItem(0)
    }

    mtd fillListFolders()
    {
        var l = treeCtrl.createLine("Shortcuts")
        l.separator = true

        visit v: dlgOptions.shortcuts
        {
            var c = Env.getSpecialDirectory(v.item0)
            var ic = getApp().getSpecialDirectoryIcon(v.item0, true)
            l = treeCtrl.createLine(v.item1, icon = ic)
            l.userData0 = cast(*void) treePathStr.count
            l.leftTextMargin = 10
            treePathStr.add(c)
        }

        l = treeCtrl.createLine("Drives")
        l.separator = true

        var volumes = catch Directory.getDrives()
        visit v: volumes
        {
            v.removeBack()
            var ic = getApp().getDirectoryIcon(v, true)
            l = treeCtrl.createLine(v.toString(), icon = ic)
            l.userData0 = cast(*void) treePathStr.count
            l.leftTextMargin = 10
            treePathStr.add(v)
        }

        treeCtrl.sigSelChanged += closure|self|(list) {
            var line = list.getSelectedLine()
            if !line
                return
            var idx = cast(u64) line.userData0
            self.setCurFolder(self.treePathStr[idx].toString())
        }
    }

    mtd fillList()
    {
        listCtrl.clear()

        with var options: EnumerationOptions
        {
            .skipAttributes = .Hidden
            .wantFiles = dlgOptions.mode != .SelectFolder
            .wantDirectories = true
            .recurse = false
        }

        curList = catch Directory.enumerate(curFolder, options)

        let item = comboType.getSelectedItem()
        var filter = dlgOptions.filters[cast(u64) item.userData0]
        var tokens = Tokenize.split(filter.item1, ";"'rune)

        visit &fi: curList
        {
            let fname = Path.getFileName(fi.fullname)

            // Filter
            var show = true
            if !fi.isDirectory() and @countof(filter.item1)
            {
                show = false
                visit t: tokens
                {
                     if Path.isExtension(fname, t)
                     {
                        show = true
                        break
                     }
                }
            }

            if !show
                continue

            // Content
            var icon = getApp().getFileDirectoryIcon(fname, fi.isDirectory())
            let line = listCtrl.createLine(fname, icon)
            line.userData0 = fi
            line.setItem(1, fi.lastWriteTime.toString(.Iso))
        }

        listCtrl.computeLayout()
        listCtrl.sort()
    }
}

impl FileDlg
{
    // Creates the message box
    func create(from: *Surface, opt: FileDlgOptions = {})->*FileDlg
    {
        let dlg = Wnd.create'FileDlg()
        with var srf = dlg.createSurface(from, 100, 100, 700, 500)
        {
            .setSurfaceFlags(add = .Sizeable)
            .minSize = {400, 400}

            if opt.title
                .setTitle(opt.title)
            else
            {
                #[Swag.Complete]
                switch opt.mode
                {
                case CreateFile:        .setTitle("Create File")
                case SelectOneFile:     .setTitle("Select File")
                case SelectMultiFiles:  .setTitle("Select Files")
                case SelectFolder:      .setTitle("Select Folder")
                }
            }
        }

        dlg.dlgOptions = opt
        dlg.addButton(opt.btnOkName orelse "Ok", Dialog.BtnOk, true)
        dlg.addButton("Cancel", Dialog.BtnCancel)
        dlg.backgroundStyle = .Window
        srf.setSurfaceFlags(remove = .CloseBtn)

        dlg.sigPressedButton = closure|dlg|(bdlg, id) {
            if id == Dialog.BtnOk
            {
                var selName = dlg.getSelectedName()
                if selName == null
                {
                    dlg.editFile.selectAll()
                    dlg.editFile.setFocus()
                    return false
                }

                if dlg.dlgOptions.mode == .CreateFile and File.exists(selName)
                {
                    var res = MessageDlg.confirm(dlg.surface, "This file already exists.", "Do you want to replace it ?")
                    return res == Dialog.BtnYes
                }
            }

            return true
        }

        // Panes
        ////////////////////////

        let view = Wnd.create'Wnd(dlg)
        view.dockStyle = .Center

        if opt.drivePane
        {
            with dlg.treeCtrl = ListCtrl.createSimple(view, {0, 0, 170})
            {
                .dockStyle = .Left
                .listFlags |= .HotTrack
                .lineHeight = 28
                .leftTextMargin = 8
                .backgroundStyle = .Dialog
                .scrollWnd.style.addStyleSheetColors("scrollBar_Bk $view_Bk")
                dlg.fillListFolders()
            }
        }

        with dlg.listCtrl = ListCtrl.createMultiColumns(view)
        {
            .selectionMode = opt.mode == .SelectMultiFiles ? .Multi : .Single
            .dockStyle = .Center
            .margin.z = 10
            .listFlags |= .HotTrack
            .backgroundStyle = .View
            .addColumn("Name", 200)
            .addColumn("Modified", 150)
            .header.form = .Transparent
            .header.style.addStyleSheetColors("headerItem_Separator $lighterMedium")
            .scrollWnd.style.addStyleSheetColors("scrollBar_Bk $view_Bk")

            .sigKeyPressed += closure|dlg|(list, evt) {
                switch evt.key
                {
                case Back:
                    dlg.folderUp()

                case Enter:
                    var l = list.getFocusLine()
                    if !l break
                    var fi = cast(const *FileInfo) l.userData0
                    if fi.isDirectory()
                    {
                        dlg.setCurFolder(fi.fullname)
                    }
                    elif dlg.dlgOptions.mode != .SelectFolder
                    {
                        dlg.validateId(Dialog.BtnOk)
                        list.setFocus()
                    }
                }
            }

            .sigLeftDoubleClick += closure|dlg|(list, pt) {
                var l = list.getSelectedLine()
                if !l return
                var fi = cast(const *FileInfo) l.userData0
                if fi.isDirectory()
                {
                    dlg.setCurFolder(fi.fullname)
                }
                elif dlg.dlgOptions.mode != .SelectFolder
                {
                    dlg.validateId(Dialog.BtnOk)
                    list.setFocus()
                }
            }

            .sigSelChanged += closure|dlg|(list) {
                var sel = list.getSelectedLines()
                var txt: String
                visit l: sel
                {
                    let fi = cast(const *FileInfo) l.userData0
                    if fi.isDirectory()
                    {
                        if dlg.dlgOptions.mode != .SelectFolder
                            continue
                    }
                    else
                    {
                        if dlg.dlgOptions.mode == .SelectFolder
                            continue
                    }

                    if !txt.isEmpty()
                        txt += ";"
                    txt += Path.getFileName(fi.fullname)
                }

                dlg.editFile.setText(txt)
            }

            .sigSort = func(l1, l2, col, down) {
                var fi1 = cast(*FileInfo) l1.userData0
                var fi2 = cast(*FileInfo) l2.userData0
                if fi1.isDirectory() and !fi2.isDirectory() return -1
                if !fi1.isDirectory() and fi2.isDirectory()   return 1
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

        // Top
        ////////////////////////
        let viewTop = Wnd.create'Wnd(view, {0, 0, 0, 50})
        viewTop.dockStyle = .Top
        viewTop.backgroundStyle = .Window
        viewTop.padding.x = 10
        viewTop.padding.z = 10
        viewTop.padding.y = 8
        viewTop.padding.w = 8
        dlg.applyLayout()

        if opt.mode == .CreateFile or opt.mode == .SelectFolder
        {
            var btn = PushButton.create(viewTop, "New Folder", {0, 0, 90})
            with btn
            {
                .dockStyle = .Right
                .margin.x = 10
                .sigPressed += closure|dlg|(btn) { dlg.onNewFolder(); }
            }
        }

        with dlg.comboPath = ComboBox.create(viewTop)
        {
            .dockStyle = .Center
            .sigChanged += closure|dlg|(combo, idx) {
                dlg.setCurFolder(dlg.comboPathStr[idx])
            }
        }

        // Bottom
        ////////////////////////
        let viewBottom = Wnd.create'Wnd(view, {0, 0, 0, 90})
        viewBottom.dockStyle = .Bottom
        viewBottom.backgroundStyle = .Window
        dlg.applyLayout()

        with dlg.editFile = EditBox.create(viewBottom, "", {105, 10, viewBottom.position.width - (105 + 15)})
        {
            .anchorStyle = .Width
        }

        with dlg.comboType = ComboBox.create(viewBottom, {105, 45, viewBottom.position.width - (105 + 15)})
        {
            .anchorStyle = .Width
            if opt.mode == .SelectFolder
                .addItem("Folders")
            else visit f, idx: opt.filters
                .addItem(f.item0, userData0 = cast(*void) idx)

            .selectItem(0)
            .sigChanged += closure|dlg|(c, idx) {
                dlg.fillList()
            }
        }

        var nameLbl0 = opt.mode == .SelectFolder ? "Folder Name" : "File Name"
        let label0 = Label.create(viewBottom, nameLbl0, {5, 10, 80, dlg.editFile.position.height})
        label0.horzAlignmentText = .Right
        label0.vertAlignmentText = .Center
        let label1 = Label.create(viewBottom, "Type", {5, 45, 80, dlg.comboType.position.height})
        label1.horzAlignmentText = .Right
        label1.vertAlignmentText = .Center

        with dlg
        {
            .setCurFolder(opt.openFolder)
            .listCtrl.sort(0)

            .editFile.setText(opt.editName)
            if opt.mode == .CreateFile
            {
                .editFile.selectAll()
                .editFile.setFocus()
            }
            else
                .listCtrl.setFocus()
        }

        // Take care of state
        if opt.state
        {
            with opt.state
            {
                if !.position.isEmpty()
                    dlg.surface.setPosition(.position)
                else
                    dlg.surface.center(from)

                if !.curFolder.isEmpty()
                    dlg.setCurFolder(.curFolder)
                else
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
        var sel = getSelectedNames()
        if sel.count == 0
            return String.from("")
        return sel.front()
    }

    // Get all selections
    mtd getSelectedNames()->Array'String
    {
        var result: retval

        let item = comboType.getSelectedItem()
        var fi = dlgOptions.filters[cast(u64) item.userData0]

        var tokens = Tokenize.split(editFile.text, ";"'rune)
        visit t: tokens
        {
            // Be sure this is a valid file/folder name
            var t1 = String.from(t)
            t1.trim()
            if t1.isEmpty()
                continue
            if dlgOptions.mode == .SelectFolder and !Path.isValidPathName(t1.toString())
                continue
            if dlgOptions.mode != .SelectFolder and !Path.isValidFileName(t1.toString())
                continue

            // Get the full name
            var val = Path.combine(curFolder, t1.toString())

            // Append extension if not done by the user
            if dlgOptions.mode != .SelectFolder and !Path.isExtension(val, fi.item1)
            {
                var exts = Tokenize.split(fi.item1, ";"'rune)
                val += exts[0]
            }

            result.add(val)
        }

        return result
    }

    private mtd onNewFolder()
    {
        var folderName: String = "New Folder"

        with var dlg = EditDlg.createSingleLine(surface)
        {
            .edit.setText(folderName)
            .edit.selectAll()
            .edit.setNote("Folder name")
            .surface.setTitle("Folder Name")
            .surface.center(surface)
        }

        dlg.sigPressedButton = closure|self, dlg|(fdlg, id) {
            if id != Dialog.BtnOk
                return true

            var folderName = dlg.edit.text
            folderName.trim()
            if folderName.isEmpty()
            {
                dlg.edit.selectAll()
                dlg.edit.setFocus()
                return false;
            }

            if !Path.isValidFileName(folderName)
            {
                MessageDlg.error(dlg.surface, "Invalid folder name")
                dlg.edit.selectAll()
                dlg.edit.setFocus()
                return false
            }

            var path = Path.combine(self.curFolder, folderName.toString())
            if Directory.exists(path)
            {
                MessageDlg.error(dlg.surface, "Folder already exists")
                dlg.edit.selectAll()
                dlg.edit.setFocus()
                return false
            }

            return true
        }

        if dlg.doModal() != Dialog.BtnOk
            return

        var path = Path.combine(curFolder, dlg.edit.text.toString())
        catch Directory.create(path)
        if @errmsg
        {
            MessageDlg.error(surface, @errmsg, "Cannot create folder")
            return
        }

        fillList()
    }
}