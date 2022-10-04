#global public
using Core, File, Directory

enum FileDlgMode
{
    CreateFile
    SelectOneFile
    SelectMultiFiles
    SelectFolder
}

struct FileDlgOptions
{
    btnOkName:  string = "Ok"
    mode:       FileDlgMode = SelectOneFile
}

struct FileDlg
{
    using dialog: Dialog
    treeCtrl:   *ListCtrl
    listCtrl:   *ListCtrl
    editFile:   *EditBox
    comboType:  *ComboBox
    dlgOptions: FileDlgOptions

    curFolder:  String
    curList:    Array'FileInfo
}

private impl FileDlg
{
    mtd fillList()
    {
        listCtrl.clear()

        var options: EnumerationOptions
        options.matchPattern = "*.*"
        options.wantFiles = dlgOptions.mode != .SelectFolder
        options.wantDirectories = true
        options.recurse = false
        curList = catch Directory.enumerate(curFolder, options)

        visit &fi: curList
        {
            name := Path.getFileName(fi.fullname)
            line := listCtrl.createLine(name)
            line.userData0 = fi
            line.setItem(1, fi.lastWriteTime.toString(.Iso))
        }

        listCtrl.computeLayout()
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
        }

        dlg.dlgOptions = opt
        dlg.addButton(opt.btnOkName, Dialog.BtnOk, true)
        dlg.addButton("Cancel", Dialog.BtnCancel)

        // Splitter
        ////////////////////////

        view := Wnd.create'Wnd(dlg)
        view.dockStyle = .Center

        with splitter := SplitterCtrl.create(view)
        {
            .backgroundStyle = .View
            .dockStyle = .Center
        }

        dlg.applyLayout()

        with dlg.treeCtrl = ListCtrl.createSimple(splitter)
        {
            .backgroundStyle = .View
            splitter.addPane(dlg.treeCtrl)
        }

        with dlg.listCtrl = ListCtrl.createMultiColumns(splitter)
        {
            .selectionMode = opt.mode == .SelectMultiFiles ? .Multi : .Single
            .addColumn("Name")
            .addColumn("Modified")
            .backgroundStyle = .View

            .sigLeftDoubleClick += @|dlg|(list, pt) {
                l := list.getSelectedLine()
                if !l return
                fi := cast(const *FileInfo) l.userData0
                dlg.curFolder = fi.fullname
                dlg.fillList()
            }

            .sigSelChanged += @|dlg|(list) {
                sel := list.getSelectedLines()
                var txt: String
                visit l: sel
                {
                    fi := cast(const *FileInfo) l.userData0
                    if fi.attributes & .Directory
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
                if (fi1.attributes & .Directory) and !(fi2.attributes & .Directory)
                    return -1
                if !(fi1.attributes & .Directory) and (fi2.attributes & .Directory)
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

            splitter.addPane(dlg.listCtrl, 500)
        }

        // Bottom
        ////////////////////////
        viewBottom := Wnd.create'Wnd(view, @{0, 0, 0, 90})
        viewBottom.dockStyle = .Bottom

        with dlg.editFile = EditBox.create(viewBottom, "", @{115, 10, 530})
        {
            .anchorStyle = .Width
        }

        with dlg.comboType = ComboBox.create(viewBottom, @{115, 45, 530})
        {
            .anchorStyle = .Width
            if opt.mode == .SelectFolder
            {
                .addItem("Folders")
                .selectItem(0)
            }
            else
            {
            }
        }

        nameLbl0 := opt.mode == .SelectFolder ? "Folder Name" : "File Name"
        label0 := Label.create(viewBottom, nameLbl0, @{5, 10, 100, dlg.editFile.position.height})
        label0.horzAlignmentText = .Right
        label0.vertAlignmentText = .Center
        label1 := Label.create(viewBottom, "Type", @{5, 45, 100, dlg.comboType.position.height})
        label1.horzAlignmentText = .Right
        label1.vertAlignmentText = .Center

        dlg.curFolder = "f:/swag-lang/swag"
        dlg.fillList()
        dlg.listCtrl.sort(0)

        return dlg
    }

    mtd getSelectedFile()->String
    {
        return Path.combine(curFolder, editFile.text.toString())
    }

    mtd getSelectedFiles()->Array'String
    {
        var result: retval

        tokens := Tokenize.split(editFile.text, ";"'rune)
        visit t: tokens
        {
            f := Path.combine(curFolder, t)
            result.add(f)
        }

        return result
    }
}