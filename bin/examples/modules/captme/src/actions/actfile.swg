using Gui

struct ActionFile {}

impl ActionFile
{
    newCmdId("SaveAsImage")
    newCmdId("SaveAsLinkedImage")
    newCmdId("OpenImage")
    newCmdId("UpdateLinkedImage")
    newCmdId("UnlinkImage")
    newCmdId("DupCapture")
    newCmdId("DeleteCapture")
    newCmdId("CaptureFileLocation")
    newCmdId("LinkedFileLocation")
    newCmdId("NewImage")
    newCmdId("NewFromClipboard")
    newCmdId("FileDetails")
}

impl IActionUI for ActionFile
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case SaveAsImage, SaveAsLinkedImage, OpenImage, FileDetails,
             UpdateLinkedImage, UnlinkImage, DupCapture, DeleteCapture,
             CaptureFileLocation, LinkedFileLocation, NewImage, NewFromClipboard:
            return true
        }

        return false
    }

    mtd impl update(cxt: ActionContext, evt: *CommandStateEvent)->bool
    {
        let main    = cast(*MainWnd) cxt.wnd
        let capture = main.getCapture()

        switch cxt.id
        {
        case NewImage:
            evt.setFlags |= .Name
            evt.name = "New Image"

        case NewFromClipboard:
            evt.setFlags |= .Name | .Disabled
            evt.name     = "New From Clipboard"
            evt.disabled = !Clipboard.hasImage()

        case SaveAsImage:
            evt.setFlags |= .Name | .Disabled | .Icon
            evt.name     = "Save As Image..."
            evt.disabled = !capture
            evt.icon     = main.getTheme().getIcon24(.SaveAs)

        case SaveAsLinkedImage:
            evt.setFlags |= .Name | .Disabled | .Icon
            evt.name     = "Save As Linked Image..."
            evt.disabled = !capture
            evt.icon     = main.getTheme().getIcon24(.Export)

        case OpenImage:
            evt.setFlags |= .Name | .Icon
            evt.name = "Open Image..."
            evt.icon = main.getTheme().getIcon24(.OpenFile)

        case UpdateLinkedImage:
            evt.setFlags |= .Name | .Disabled | .Icon
            evt.name     = "Update Linked Image"
            evt.disabled = !capture
            evt.icon     = main.getTheme().getIcon24(.Update)

        case UnlinkImage:
            evt.setFlags |= .Name | .Disabled
            evt.name     = "Unlink Image"
            evt.disabled = !capture or capture.linkedImage.length == 0

        case DeleteCapture:
            evt.setFlags |= .Name | .Disabled | .Icon
            evt.name = "Delete"
            if main.libraryMode do
                evt.disabled = !main.libWnd.hasSelected()
            else do
                evt.disabled = !capture
            evt.icon = main.getTheme().getIcon24(.Remove)

        case DupCapture:
            evt.setFlags |= .Name | .Disabled | .Icon
            evt.name     = "Duplicate"
            evt.disabled = !capture
            evt.icon     = main.getTheme().getIcon24(.Duplicate)

        case CaptureFileLocation:
            evt.setFlags |= .Name | .Disabled | .Icon
            evt.name     = "Locate Capture File"
            evt.disabled = !capture
            evt.icon     = main.getTheme().getIcon24(.OpenFile)

        case LinkedFileLocation:
            evt.setFlags |= .Name | .Disabled | .Icon
            evt.name     = "Locate Linked Image"
            evt.disabled = !capture or capture.linkedImage.isEmpty()
            evt.icon     = main.getTheme().getIcon24(.Link)

        case FileDetails:
            evt.setFlags |= .Name | .Disabled
            evt.name = "File Details"
            if main.libraryMode do
                evt.disabled = !main.libWnd.hasSelected()
            else do
                evt.disabled = !capture
        }

        return true
    }

    mtd impl execute(cxt: ActionContext)->bool
    {
        let main    = cast(*MainWnd) cxt.wnd
        let capture = assume main.getCapture()

        switch cxt.id
        {
        case NewImage:
            main.newImage()

        case NewFromClipboard:
            main.newCapture(Clipboard.getImage())

        case SaveAsImage:
            main.saveAsImage()

        case SaveAsLinkedImage:
            main.saveAsImage(linked: true)

        case OpenImage:
            main.openImage()

        case UnlinkImage:
            let res = MessageDlg.confirm(main.surface, "This will remove the image association.")
            if res == MessageDlg.BtnYes
            {
                capture.linkedImage.clear()
                capture.changed        = true
                capture.linkedUpToDate = false
                catch capture.save()
                main.recentWnd.updateRecent()
            }

        case UpdateLinkedImage:
            if capture.linkedImage.length == 0
            {
                main.saveAsImage(linked: true)
                capture.linkedUpToDate = true
                catch capture.save()
                main.recentWnd.updateRecent()
                break
            }

            if main.editorOptions.confirmLinkedOverwrite
            {
                var txt = Format.toString("This will overwrite the original image.\n")
                txt += Format.toString("<i>%</i>", capture.linkedImage)

                var opt: MessageDlgOptions
                opt.sigCreate = func|main|(dlg)
                {
                    let chk = CheckButton.create(dlg.surface.getView(), "Do not ask again", {0, 0, 100, 40})
                    chk.sigChanged += func|main|(btn)
                    {
                        main.editorOptions.confirmLinkedOverwrite = !btn.isChecked()
                    }
                    dlg.setExtension(chk)
                }

                if MessageDlg.confirm(main.surface, txt, options: opt) != Dialog.BtnYes do
                    break
            }

            Cursor.wait()
            var img = capture.toImage()
            main.doSaveImage(capture, &img, capture.linkedImage)

        case DeleteCapture:
            let res = MessageDlg.confirm(main.surface, "This will delete the capture file(s).")
            if res == MessageDlg.BtnYes
            {
                if main.libraryMode do
                    main.libWnd.removeSelected()
                else do
                    main.recentWnd.removeRecent()
            }

        case DupCapture:
            Cursor.wait()
            catch capture.save()

            let file = Library.newFileName()
            trycatch File.duplicate(file, capture.file)

            let newCapture = trycatch Capture.load(file)
            newCapture.file        = file
            newCapture.linkedImage = null
            newCapture.changed     = true
            trycatch newCapture.save()
            Memory.delete(newCapture)
            g_Library.addFile(file)

            let dupCapture = trycatch main.recentWnd.addRecent(file, true, true)
            Memory.delete(dupCapture)

        case CaptureFileLocation:
            Env.locateInExplorer(capture.file)

        case LinkedFileLocation:
            Env.locateInExplorer(capture.linkedImage)

        case FileDetails:

            func addLine(lt: *Wnd, title, val: string)
            {
                let wnd = Wnd.create'StackLayoutCtrl(lt)
                wnd.layoutKind      = .Left
                wnd.position.height = 20
                let lbl1 = Label.create(wnd, title, {0, 0, 100, 0})
                lbl1.horzAlignmentText = .Left
                let lbl2 = Label.create(wnd, val, {0, 0, 1000, 0})
                lbl2.horzAlignmentText = .Left
            }

            let dlg = Dialog.createDialog(main.surface, 600, 400, title: "Details", flags: .Border | .CloseBtn | .Sizeable)
            dlg.addButton("OK", Dialog.BtnOk, true)
            let lt = StackLayoutCtrl.create(dlg)
            lt.dockStyle = .Center
            lt.padding   = 5
            addLine(lt, "<b>File<b>", Format.toString("%", capture.file))
            if capture.linkedImage != null do
                addLine(lt, "<b>Linked Image<b>", Format.toString("%", capture.linkedImage))
            addLine(lt, "<b>Image Size<b>", Format.toString("%x%", capture.width, capture.height))
            addLine(lt, "<b>File Size<b>", StrConv.toDisplaySize(catch File.getInfo(capture.file).size))
            lt.computeLayout()
            discard dlg.doModal()
        }

        return true
    }
}

impl MainWnd
{
    mtd openImage()
    {
        var opt: FileDlgOptions
        with opt
        {
            .state     = &me.fileDlgState
            .mode      = .SelectOneFile
            .title     = "Load Image"
            .btnOkName = "Load"
            .shortcuts.add({Env.SpecialDirectory.Desktop, "Desktop"})
            .shortcuts.add({Env.SpecialDirectory.UserPictures, "My Images"})
            .shortcuts.add({Env.SpecialDirectory.CommonPictures, "Images"})
            .filters.add({"PNG - Portable Network Graphics (*.png)", ".png"})
            .filters.add({"JPG - JPEG Image (*.jpg)", ".jpg"})
            .filters.add({"BMP - Windows Bitmap (*.bmp)", ".bmp"})
            .filters.add({"TGA - Truevision Targa (*.tga)", ".tga"})
        }

        let dlg = FileDlg.create(.surface, opt)
        if dlg.doModal() != Dialog.BtnOk do
            return

        let img = catch Image.load(dlg.getSelectedName())
        if @err != null do
            MessageDlg.error(.surface, "Failed to load image")
        else do
            .newCapture(img, dlg.getSelectedName())
    }

    mtd doSaveImage(capture: *Capture, img: *Image, nameFile: string)
    {
        var optImg: any
        var optPng: Png.EncodeOptions
        var optJpg: Jpg.EncodeOptions

        if Path.getExtensionLowerCase(nameFile) == ".png"
        {
            optPng.compressionLevel = Compress.Deflate.CompressionLevel.BestSpeed
            optPng.filtering        = false
            optImg                  = optPng
        }
        elif Path.getExtensionLowerCase(nameFile) == ".jpg"
        {
            optJpg.quality = 95
            optImg         = optJpg
        }

        var md: ImageMetaData
        md.tag   = "Captme.File"
        md.value = capture.file.toSlice()
        img.metaDatas.add(md)

        catch img.save(nameFile, optImg)
        if @err != null
        {
            MessageDlg.error(.surface, "Failed to save file")
            return
        }

        capture.changed        = true
        capture.linkedUpToDate = true
        catch capture.save()
        if .recentWnd do
            .recentWnd.updateRecent()

        let txt = Format.toString("Image <url>%</url> has been saved", nameFile)
        let emb = EmbInfoCtrl.create(.editWnd.view, txt, .Information, flags: .Bottom | .AutoHide)
        emb.label.sigUrlClicked += func(lbl, msg)
        {
            Env.locateInExplorer(msg)
        }
    }

    mtd saveAsImage(linked = false)
    {
        let fileName = .getSaveAsImageName()
        if fileName.isEmpty() do
            return
        .doSaveAsImage(fileName, linked)
    }

    mtd getSaveAsImageName()->String
    {
        let capture = assume .getCapture()

        var opt: FileDlgOptions
        with opt
        {
            .state     = &me.fileDlgState
            .mode      = .CreateFile
            .title     = "Save As Image"
            .btnOkName = "Save"
            .shortcuts.add({Env.SpecialDirectory.Desktop, "Desktop"})
            .shortcuts.add({Env.SpecialDirectory.UserPictures, "My Images"})
            .shortcuts.add({Env.SpecialDirectory.CommonPictures, "Images"})
            .editName = Path.getFileNameWithoutExtension(capture.file)
            .filters.add({"PNG - Portable Network Graphics (*.png)", ".png"})
            .filters.add({"JPG - JPEG Image (*.jpg)", ".jpg"})
            .filters.add({"BMP - Windows Bitmap (*.bmp)", ".bmp"})
            .filters.add({"TGA - Truevision Targa (*.tga)", ".tga"})
        }

        let dlg = FileDlg.create(.surface, opt)
        if .inPlaceEdit do
            dlg.surface.setTopMost()
        if dlg.doModal() != Dialog.BtnOk do
            return ""
        return dlg.getSelectedName()
    }

    mtd doSaveAsImage(fileName: String, linked = false)
    {
        let capture = assume .getCapture()
        Cursor.wait()
        var img = capture.toImage()
        if linked do
            capture.linkedImage = fileName
        .doSaveImage(capture, &img, fileName)
    }

    mtd newImage()
    {
        let dlg = Dialog.createDialog(.surface, 350, 235, title: "New Image")
        dlg.addButton("Create", Dialog.BtnOk, true)
        dlg.addButton("Cancel", Dialog.BtnCancel)
        let edit0 = EditCtrl.create(dlg, "Width", "512", {90, 12}, flags: .RightAlign)
        let edit1 = EditCtrl.create(dlg, "Height", "512", {90, 47}, flags: .RightAlign)
        edit0.edit.setInputU64(16, 4096)
        edit1.edit.setInputU64(16, 4096)
        if dlg.doModal() != Dialog.BtnOk do
            return

        let (w, ?, ?) = catch StrConv.parseS32(edit0.edit.text)
        let (h, ?, ?) = catch StrConv.parseS32(edit1.edit.text)
        var img = Image.create(w orelse 512, h orelse 512, .RGB8)
        img.fill(Argb.White)
        .newCapture(img)
    }
}
