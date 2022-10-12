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
    filters:    Array'{string, string}
    shortcuts:  Array'{Env.SpecialDirectory, string}
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
    mtd onDestroyEvent(evt: *DestroyEvent)
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
        tt := curFolder

        if curFolder.back() == "/"'u8
            curFolder.removeBack()

        fillList()
        fillComboPath()
    }

    mtd fillComboPath()
    {
        comboPath.clear()
        comboPathStr.clear()

        all := Tokenize.split(curFolder.toString(), "/"'rune)
        loop i: all.count-1..0
        {
            var str: String
            loop j: i+1
            {
                str += all[j]
                str += "/"'u8
            }

            comboPathStr.add(str)

            one := all[i]
            ic := getApp().getDirectoryIcon(one)
            comboPath.addItem(one, icon: ic)
        }

        comboPath.selectItem(0)
    }

    mtd fillListFolders()
    {
        visit v: dlgOptions.shortcuts
        {
            c := Env.getSpecialDirectory(v.item0)
            ic := getApp().getSpecialDirectoryIcon(v.item0, true)
            l := treeCtrl.createLine(v.item1, icon: ic)
            l.userData0 = cast(*void) treePathStr.count
            treePathStr.add(c)
        }

        volumes := catch Directory.getDrives()
        visit v: volumes
        {
            v1 := dref v
            v1.removeBack()
            ic := getApp().getDirectoryIcon(v1, true)
            l := treeCtrl.createLine(v.toString(), icon: ic)
            l.userData0 = cast(*void) treePathStr.count
            treePathStr.add(dref v)
        }

        treeCtrl.sigSelChanged += @|self|(list) {
            line := list.getSelectedLine()
            if !line
                return
            idx := cast(uint) line.userData0
            self.setCurFolder(self.treePathStr[idx].toString())
        }
    }

    mtd fillList()
    {
        listCtrl.clear()

        var options: EnumerationOptions
        options.matchPattern = "*.*"
        options.skipAttributes = .Hidden
        options.wantFiles = dlgOptions.mode != .SelectFolder
        options.wantDirectories = true
        options.recurse = false
        curList = catch Directory.enumerate(curFolder, options)

        item := comboType.getSelectedItem()
        filter := dlgOptions.filters[cast(uint) item.userData0]
        tokens := Tokenize.split(filter.item1, ";"'rune)

        visit &fi: curList
        {
            fname := Path.getFileName(fi.fullname)

            // Filter
            show := true
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
            icon := getApp().getFileDirectoryIcon(fname, fi.isDirectory())
            line := listCtrl.createLine(fname, icon)
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
    func create(from: *Surface, opt: FileDlgOptions = @{})->*FileDlg
    {
        dlg := Wnd.create'FileDlg()
        with srf := dlg.createSurface(from, 100, 100, 700, 500)
        {
            .setSurfaceFlags(add: .Sizeable)
            .minSize = @{400, 400}

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
        srf.setSurfaceFlags(remove: .CloseBtn)

        dlg.sigPressedButton = @|dlg|(bdlg, id) {
            if id == Dialog.BtnOk
            {
                selName := dlg.getSelectedName()
                if selName == null
                {
                    dlg.editFile.selectAll()
                    dlg.editFile.setFocus()
                    return false
                }

                if dlg.dlgOptions.mode == .CreateFile and File.exists(selName)
                {
                    res := MessageDlg.confirm(dlg.surface, "This file already exists.", "Do you want to replace it ?")
                    return res == Dialog.BtnYes
                }
            }

            return true
        }

        // Splitter
        ////////////////////////

        view := Wnd.create'Wnd(dlg)
        view.dockStyle = .Center

        if opt.drivePane
        {
            with dlg.treeCtrl = ListCtrl.createSimple(view, @{0, 0, 170})
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

            .sigKeyPressed += @|dlg|(list, evt) {
                switch evt.key
                {
                case Back:
                    dlg.folderUp()

                case Enter:
                    l := list.getFocusLine()
                    if !l break
                    fi := cast(const *FileInfo) l.userData0
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

            .sigLeftDoubleClick += @|dlg|(list, pt) {
                l := list.getSelectedLine()
                if !l return
                fi := cast(const *FileInfo) l.userData0
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

            .sigSelChanged += @|dlg|(list) {
                sel := list.getSelectedLines()
                var txt: String
                visit l: sel
                {
                    fi := cast(const *FileInfo) l.userData0
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

            .sigSort = @(l1, l2, col, down) {
                fi1 := cast(*FileInfo) l1.userData0
                fi2 := cast(*FileInfo) l2.userData0
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
        viewTop := Wnd.create'Wnd(view, @{0, 0, 0, 50})
        viewTop.dockStyle = .Top
        viewTop.backgroundStyle = .Window
        viewTop.padding.x = 10
        viewTop.padding.z = 10
        viewTop.padding.y = 8
        viewTop.padding.w = 8
        dlg.applyLayout()

        btn := PushButton.create(viewTop, "New Folder", @{0, 0, 90})
        with btn
        {
            .dockStyle = .Right
            .margin.x = 10
            .sigPressed += @|dlg|(btn) { dlg.onNewFolder(); }
        }

        with dlg.comboPath = ComboBox.create(viewTop)
        {
            .dockStyle = .Center
            .sigChanged += @|dlg|(combo, idx) {
                dlg.setCurFolder(dref dlg.comboPathStr[idx])
            }
        }

        // Bottom
        ////////////////////////
        viewBottom := Wnd.create'Wnd(view, @{0, 0, 0, 90})
        viewBottom.dockStyle = .Bottom
        viewBottom.backgroundStyle = .Window
        dlg.applyLayout()

        with dlg.editFile = EditBox.create(viewBottom, "", @{105, 10, viewBottom.position.width - (105 + 15)})
        {
            .anchorStyle = .Width
        }

        with dlg.comboType = ComboBox.create(viewBottom, @{105, 45, viewBottom.position.width - (105 + 15)})
        {
            .anchorStyle = .Width
            if opt.mode == .SelectFolder
                .addItem("Folders")
            else visit f, idx: opt.filters
                .addItem(f.item0, userData0: cast(*void) idx)

            .selectItem(0)
            .sigChanged += @|dlg|(c, idx) {
                dlg.fillList()
            }
        }

        nameLbl0 := opt.mode == .SelectFolder ? "Folder Name" : "File Name"
        label0 := Label.create(viewBottom, nameLbl0, @{5, 10, 80, dlg.editFile.position.height})
        label0.horzAlignmentText = .Right
        label0.vertAlignmentText = .Center
        label1 := Label.create(viewBottom, "Type", @{5, 45, 80, dlg.comboType.position.height})
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
        sel := getSelectedNames()
        if sel.count == 0
            return String.from("")
        return sel.front()
    }

    // Get all selections
    mtd getSelectedNames()->Array'String
    {
        var result: retval

        item := comboType.getSelectedItem()
        fi := dlgOptions.filters[cast(uint) item.userData0]

        tokens := Tokenize.split(editFile.text, ";"'rune)
        visit t: tokens
        {
            // Be sure this is a valid file/folder name
            t1 := String.from(t)
            t1.trim()
            if t1.isEmpty()
                continue
            if dlgOptions.mode == .SelectFolder and !Path.isValidPathName(t1.toString())
                continue
            if dlgOptions.mode != .SelectFolder and !Path.isValidFileName(t1.toString())
                continue

            // Get the full name
            val := Path.combine(curFolder, t1.toString())

            // Append extension if not done by the user
            if dlgOptions.mode != .SelectFolder and !Path.isExtension(val, fi.item1)
            {
                exts := Tokenize.split(fi.item1, ";"'rune)
                val += exts[0]
            }

            result.add(val)
        }

        return result
    }

    private mtd onNewFolder()
    {
        var folderName: String = "New Folder"

        dlg := EditDlg.create(surface)
        dlg.edit.setText(folderName)
        dlg.edit.selectAll()
        dlg.edit.setNote("Folder name")
        dlg.surface.setTitle("Folder Name")
        dlg.surface.center(surface)

        dlg.sigPressedButton = @|self, dlg|(fdlg, id) {
            if id != Dialog.BtnOk
                return true

            folderName := dlg.edit.text
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

            path := Path.combine(self.curFolder, folderName.toString())
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

        path := Path.combine(curFolder, dlg.edit.text.toString())
        catch Directory.create(path)
        if @err
        {
            MessageDlg.error(surface, @err, "Cannot create folder")
            return
        }

        fillList()
    }
}