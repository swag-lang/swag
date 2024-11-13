using Gui

const CmdIdSaveAsImage         = "CmdIdSaveAsImage"
const CmdIdSaveAsLinkedImage   = "CmdIdSaveAsLinkedImage"
const CmdIdOpenImage           = "CmdIdOpenImage"
const CmdIdUpdateLinkedImage   = "CmdIdUpdateLinkedImage"
const CmdIdUnlinkImage         = "CmdIdUnlinkImage"
const CmdIdDupCapture          = "CmdIdDupCapture"
const CmdIdDeleteCapture       = "CmdIdDeleteCapture"
const CmdIdCaptureFileLocation = "CmdIdCaptureFileLocation"
const CmdIdLinkedFileLocation  = "CmdIdLinkedFileLocation"
const CmdIdNewImage            = "CmdIdNewImage"
const CmdIdNewFromClipboard    = "CmdIdNewFromClipboard"
const CmdIdFileDetails         = "CmdIdFileDetails"

struct ActionFile {}
impl IActionUI for ActionFile
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case CmdIdSaveAsImage, CmdIdSaveAsLinkedImage, CmdIdOpenImage, CmdIdFileDetails,
             CmdIdUpdateLinkedImage, CmdIdUnlinkImage, CmdIdDupCapture, CmdIdDeleteCapture,
             CmdIdCaptureFileLocation, CmdIdLinkedFileLocation, CmdIdNewImage, CmdIdNewFromClipboard:
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
        case CmdIdNewImage:
            evt.setFlags |= .Name
            evt.name = "New Image"

        case CmdIdNewFromClipboard:
            evt.setFlags |= .Name | .Disabled
            evt.name     = "New From Clipboard"
            evt.disabled = !Clipboard.hasImage()

        case CmdIdSaveAsImage:
            evt.setFlags |= .Name | .Disabled | .Icon
            evt.name     = "Save As Image..."
            evt.disabled = !capture
            evt.icon     = main.getTheme().getIcon24(.SaveAs)

        case CmdIdSaveAsLinkedImage:
            evt.setFlags |= .Name | .Disabled | .Icon
            evt.name     = "Save As Linked Image..."
            evt.disabled = !capture
            evt.icon     = main.getTheme().getIcon24(.Export)

        case CmdIdOpenImage:
            evt.setFlags |= .Name | .Icon
            evt.name = "Open Image..."
            evt.icon = main.getTheme().getIcon24(.OpenFile)

        case CmdIdUpdateLinkedImage:
            evt.setFlags |= .Name | .Disabled | .Icon
            evt.name     = "Update Linked Image"
            evt.disabled = !capture
            evt.icon     = main.getTheme().getIcon24(.Update)

        case CmdIdUnlinkImage:
            evt.setFlags |= .Name | .Disabled
            evt.name     = "Unlink Image"
            evt.disabled = !capture or capture.linkedImage.length == 0

        case CmdIdDeleteCapture:
            evt.setFlags |= .Name | .Disabled | .Icon
            evt.name = "Delete"
            if main.libraryMode:
                evt.disabled = !main.libWnd.hasSelected()
            else:
                evt.disabled = !capture
            evt.icon = main.getTheme().getIcon24(.Remove)

        case CmdIdDupCapture:
            evt.setFlags |= .Name | .Disabled | .Icon
            evt.name     = "Duplicate"
            evt.disabled = !capture
            evt.icon     = main.getTheme().getIcon24(.Duplicate)

        case CmdIdCaptureFileLocation:
            evt.setFlags |= .Name | .Disabled | .Icon
            evt.name     = "Locate Capture File"
            evt.disabled = !capture
            evt.icon     = main.getTheme().getIcon24(.OpenFile)

        case CmdIdLinkedFileLocation:
            evt.setFlags |= .Name | .Disabled | .Icon
            evt.name     = "Locate Linked Image"
            evt.disabled = !capture or capture.linkedImage.isEmpty()
            evt.icon     = main.getTheme().getIcon24(.Link)

        case CmdIdFileDetails:
            evt.setFlags |= .Name | .Disabled
            evt.name = "File Details"
            if main.libraryMode:
                evt.disabled = !main.libWnd.hasSelected()
            else:
                evt.disabled = !capture
        }

        return true
    }

    mtd impl execute(cxt: ActionContext)->bool
    {
        let main    = cast(*MainWnd) cxt.wnd
        let capture = main.getCapture()

        switch cxt.id
        {
        case CmdIdNewImage:
            main.newImage()

        case CmdIdNewFromClipboard:
            main.newCapture(Clipboard.getImage())

        case CmdIdSaveAsImage:
            main.saveAsImage()

        case CmdIdSaveAsLinkedImage:
            main.saveAsImage(linked: true)

        case CmdIdOpenImage:
            main.openImage()

        case CmdIdUnlinkImage:
            let res = MessageDlg.confirm(main.surface, "This will remove the image association.")
            if res == MessageDlg.BtnYes
            {
                capture.linkedImage.clear()
                capture.changed = true
                capture.linkedUpToDate = false
                catch capture.save()
                main.recentWnd.updateRecent()
            }

        case CmdIdUpdateLinkedImage:
            if capture.linkedImage.length == 0
            {    
                main.saveAsImage(linked: true)
                capture.linkedUpToDate = true
                catch capture.save()
                main.recentWnd.updateRecent()
                break            
            }

            if g_EditorOptions.confirmLinkedOverwrite
            {
                var txt = Format.toString("This will overwrite the original image.\n")
                txt += Format.toString("<i>%</i>", capture.linkedImage)

                var opt: MessageDlgOptions
                opt.sigCreate = func(dlg)
                {
                    let chk = CheckButton.create(dlg.surface.getView(), "Do not ask again", {0, 0, 100, 40})
                    chk.sigChanged += func(btn)
                    {
                        g_EditorOptions.confirmLinkedOverwrite = !btn.isChecked()
                    }
                    dlg.setExtension(chk)
                }

                if MessageDlg.confirm(main.surface, txt, options: opt) != Dialog.BtnYes:
                    break
            }

            Cursor.wait()
            var img = capture.toImage()
            catch img.save(capture.linkedImage)
            if @err() != null:
                MessageDlg.error(main.surface, "Failed to update file")
            else
            {
                var txt = Format.toString("Image <i>%</i> has been saved", capture.linkedImage)
                discard EmbInfoCtrl.create(main.editWnd.view, txt, .Information, flags: .Bottom | .AutoHide)
            }

            capture.linkedUpToDate = true
            catch capture.save()
            main.recentWnd.updateRecent()


        case CmdIdDeleteCapture:
            let res = MessageDlg.confirm(main.surface, "This will delete the capture file(s).")
            if res == MessageDlg.BtnYes
            {
                if main.libraryMode:
                    main.libWnd.removeSelected()
                else:
                    main.recentWnd.removeRecent()
            }

        case CmdIdDupCapture:
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

        case CmdIdCaptureFileLocation:
            Env.locateInExplorer(capture.file)

        case CmdIdLinkedFileLocation:
            Env.locateInExplorer(capture.linkedImage)

        case CmdIdFileDetails:
            var file: *Capture
            if main.libraryMode:
                file = main.libWnd.getCapture()
            else:
                file = capture
            if !file:
                break

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
            addLine(lt, "<b>File<b>", Format.toString("%", file.file))
            if file.linkedImage != null:
                addLine(lt, "<b>Linked Image<b>", Format.toString("%", file.linkedImage))
            addLine(lt, "<b>Size<b>", Format.toString("%x%", file.width, file.height))
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
            .state     = &fileDlgState
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

        let dlg = FileDlg.create(surface, opt)
        if dlg.doModal() != Dialog.BtnOk:
            return

        let img = catch Image.load(dlg.getSelectedName())
        if @err() != null:
            MessageDlg.error(surface, "Failed to load image")
        else:
            newCapture(img, dlg.getSelectedName())
    }

    mtd saveAsImage(linked = false)
    {
        var opt: FileDlgOptions
        with opt
        {
            .state     = &fileDlgState
            .mode      = .CreateFile
            .title     = "Save As Image"
            .btnOkName = "Save"
            .shortcuts.add({Env.SpecialDirectory.Desktop, "Desktop"})
            .shortcuts.add({Env.SpecialDirectory.UserPictures, "My Images"})
            .shortcuts.add({Env.SpecialDirectory.CommonPictures, "Images"})
            .editName = Path.getFileNameWithoutExtension(getCapture().file)
            .filters.add({"PNG - Portable Network Graphics (*.png)", ".png"})
            .filters.add({"JPG - JPEG Image (*.jpg)", ".jpg"})
            .filters.add({"BMP - Windows Bitmap (*.bmp)", ".bmp"})
            .filters.add({"TGA - Truevision Targa (*.tga)", ".tga"})
        }

        let dlg = FileDlg.create(surface, opt)
        if dlg.doModal() != Dialog.BtnOk:
            return

        Cursor.wait()
        let capture = getCapture()
        var img     = capture.toImage()
        catch img.save(dlg.getSelectedName())
        if @err() != null
        {
            MessageDlg.error(surface, "Failed to save file")
            return
        }

        var txt = Format.toString("Image <i>%</i> has been saved", dlg.getSelectedName())
        discard EmbInfoCtrl.create(editWnd.view, txt, .Information, .Bottom | .AutoHide)

        if linked
        {
            capture.linkedImage = dlg.getSelectedName()
            capture.changed     = true
            catch capture.save()
            recentWnd.updateRecent()
        }
    }

    mtd newImage()
    {
        let dlg = Dialog.createDialog(surface, 350, 235, title: "New Image")
        dlg.addButton("Create", Dialog.BtnOk, true)
        dlg.addButton("Cancel", Dialog.BtnCancel)
        let edit0 = EditCtrl.create(dlg, "Width", "512", {90, 12}, flags: .RightAlign)
        let edit1 = EditCtrl.create(dlg, "Height", "512", {90, 47}, flags: .RightAlign)
        edit0.edit.setInputU64(16, 4096)
        edit1.edit.setInputU64(16, 4096)
        if dlg.doModal() != Dialog.BtnOk:
            return

        let (w, ?, ?) = catch StrConv.parseS32(edit0.edit.text)
        let (h, ?, ?) = catch StrConv.parseS32(edit1.edit.text)
        var img = Image.create(w orelse 512, h orelse 512, .RGB8)
        img.fill(Argb.White)
        newCapture(img)
    }
}
