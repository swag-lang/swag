#global public
using Core, File, Directory

enum FileDlgMode
{
    SelectOneFile
    SelectMultiFiles
    SelectFolder
}

struct FileDlgOptions
{
    multiSelection: bool
    mode:           FileDlgMode = SelectOneFile
}

struct FileDlg
{
    using dialog: Dialog
    treeCtrl:   *ListCtrl
    listCtrl:   *ListCtrl
    editFile:   *EditBox
    comboType:  *ComboBox

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
        options.wantFiles = true
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
            .selectionMode = opt.multiSelection ? .Multi : .Single
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
                    if !txt.isEmpty()
                        txt += ";"
                    fi := cast(const *FileInfo) l.userData0
                    txt += Path.getFileName(fi.fullname)
                }

                dlg.editFile.setText(txt)
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
        }

        label0 := Label.create(viewBottom, "File Name", @{5, 10, 100, dlg.editFile.position.height})
        label0.horzAlignmentText = .Right
        label0.vertAlignmentText = .Center
        label1 := Label.create(viewBottom, "Type", @{5, 45, 100, dlg.comboType.position.height})
        label1.horzAlignmentText = .Right
        label1.vertAlignmentText = .Center

        dlg.curFolder = "f:/swag-lang/swag"
        dlg.fillList()

        return dlg
    }
}