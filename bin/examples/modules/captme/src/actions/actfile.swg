using Gui

const CmdIdSaveAsImage          = "CmdIdSaveAsImage"
const CmdIdSaveAsLinkedImage    = "CmdIdSaveAsLinkedImage"
const CmdIdOpenImage            = "CmdIdOpenImage"
const CmdIdUpdateLinkedImage    = "CmdIdUpdateLinkedImage"
const CmdIdDupCapture           = "CmdIdDupCapture"
const CmdIdDeleteCapture        = "CmdIdDeleteCapture"
const CmdIdFileLocation         = "CmdIdFileLocation"
const CmdIdNewImage             = "CmdIdNewImage"

struct ActionFile {}
impl IActionUI for ActionFile
{
    mtd accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case CmdIdSaveAsImage,
             CmdIdSaveAsLinkedImage,
             CmdIdOpenImage,
             CmdIdUpdateLinkedImage,
             CmdIdDupCapture,
             CmdIdDeleteCapture,
             CmdIdFileLocation:
             return true
        }

        return false
    }

    mtd update(cxt: ActionContext, evt: *CommandUIEvent)->bool
    {
        main := cast(*MainWnd) cxt.wnd
        capture := main.getCapture()

        switch cxt.id
        {
        case CmdIdNewImage:
            evt.setFlags |= .Name
            evt.name = "New Image"

        case CmdIdSaveAsImage:
            evt.setFlags |= .Name | .Disabled
            evt.name = "Save As Image..."
            evt.disabled = !capture

        case CmdIdSaveAsLinkedImage:
            evt.setFlags |= .Name | .Disabled
            evt.name = "Save As Linked Image..."
            evt.disabled = !capture

        case CmdIdOpenImage:
            evt.setFlags |= .Name
            evt.name = "Open Image..."

        case CmdIdUpdateLinkedImage:
            evt.setFlags |= .Name | .Disabled
            evt.name = "Update Linked Image"
            evt.disabled = !capture or capture.linkedImage.length == 0

        case CmdIdDeleteCapture:
            evt.setFlags |= .Name | .Disabled
            evt.name = "Delete"
            evt.disabled = !capture

        case CmdIdDupCapture:
            evt.setFlags |= .Name | .Disabled
            evt.name = "Duplicate"
            evt.disabled = !capture

        case CmdIdFileLocation:
            evt.setFlags |= .Name | .Disabled
            evt.name = "Open File Location"
            evt.disabled = !capture
        }

        return true
    }

    mtd execute(cxt: ActionContext)->bool
    {
        main := cast(*MainWnd) cxt.wnd
        capture := main.getCapture()

        switch cxt.id
        {
        case CmdIdNewImage:
            main.newImage()

        case CmdIdSaveAsImage:
            main.saveAsImage()

        case CmdIdSaveAsLinkedImage:
            main.saveAsImage(linked: true)

        case CmdIdOpenImage:
            main.openImage()

        case CmdIdUpdateLinkedImage:
            txt := Format.toString("This will overwrite the original image.\n")
            txt += Format.toString("File is '%'", capture.linkedImage)
            if MessageDlg.confirm(main.surface, txt) != Dialog.BtnYes
                break
            Cursor.wait()
            img := capture.toImage()
            catch img.save(capture.linkedImage)
            if @err
                MessageDlg.error(main.surface, @err, "Failed to update file !")

        case CmdIdDeleteCapture:
            res := MessageDlg.confirm(main.surface, "This will delete the capture file.")
            if res == MessageDlg.BtnYes
                main.recentWnd.removeRecent()

        case CmdIdDupCapture:
            file := Library.newFileName()
            trycatch File.duplicate(file, capture.file)
            dupCapture := trycatch main.recentWnd.addRecent(file)
            Memory.delete(dupCapture)
            main.recentWnd.recentView.select(0)
            g_Library.addFile(file)

        case CmdIdFileLocation:
            File.locateInExplorer(capture.file)
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
            .state = &fileDlgState
            .mode = .SelectOneFile
            .title = "Load Image"
            .btnOkName = "Load"
            .shortcuts.add({Env.SpecialDirectory.Desktop, "Desktop"})
            .shortcuts.add({Env.SpecialDirectory.UserPictures, "My Images"})
            .shortcuts.add({Env.SpecialDirectory.CommonPictures, "Images"})
            .filters.add({"PNG - Portable Network Graphics (*.png)", ".png"})
            .filters.add({"JPG - JPEG Image (*.jpg)", ".jpg"})
            .filters.add({"BMP - Windows Bitmap (*.bmp)", ".bmp"})
            .filters.add({"TGA - Truevision Targa (*.tga)", ".tga"})
        }

        dlg := FileDlg.create(surface, opt)
        if dlg.doModal() != Dialog.BtnOk
            return

        img := catch Image.load(dlg.getSelectedName())
        if @err
            MessageDlg.error(surface, @err, "Failed to load image !")
        else
            newCapture(img, dlg.getSelectedName())
    }

    mtd saveAsImage(linked = false)
    {
        var opt: FileDlgOptions
        with opt
        {
            .state = &fileDlgState
            .mode = .CreateFile
            .title = "Save As Image"
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

        dlg := FileDlg.create(surface, opt)
        if dlg.doModal() != Dialog.BtnOk
            return

        Cursor.wait()
        capture := getCapture()
        img := capture.toImage()
        catch img.save(dlg.getSelectedName())
        if @err
        {
            MessageDlg.error(surface, @err, "Failed to save file !")
            return
        }

        if linked
        {
            capture.linkedImage = dlg.getSelectedName()
            capture.changed = true
            catch capture.save()
        }
    }

    mtd newImage()
    {
        dlg := Dialog.createDialog(surface, 350, 235, title: "New Image")
        dlg.addButton("Create", Dialog.BtnOk, true)
        dlg.addButton("Cancel", Dialog.BtnCancel)
        edit0 := EditCtrl.create(dlg, "Width", "512", {90, 12}, flags: .RightAlign)
        edit1 := EditCtrl.create(dlg, "Height", "512", {90, 47}, flags: .RightAlign)
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
}