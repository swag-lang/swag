struct ActionImage {}

var g_KeepRatio:  bool = true
var g_ResizeMode: u64 = 5
var g_Width, g_Height: s32

impl ActionImage
{
    newCmdId("TrimCapture")
    newCmdId("ResizeImage")
    newCmdId("ExpandImage")
    newCmdId("ExpandCapture")
    newCmdId("RestoreOrg")
    newCmdId("FlipImage")
    newCmdId("MirrorImage")
    newCmdId("RotateImageRight")
    newCmdId("RotateImageLeft")
}

impl IActionUI for ActionImage
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case TrimCapture, ResizeImage, ExpandImage,
             ExpandCapture, RestoreOrg,
             FlipImage, MirrorImage,
             RotateImageRight, RotateImageLeft:
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
        case ResizeImage:
            evt.setFlags |= .Name | .Disabled
            evt.name     = "Resize Image..."
            evt.disabled = !capture

        case ExpandImage:
            evt.setFlags |= .Name | .Disabled
            evt.name     = "Expand Image..."
            evt.disabled = !capture

        case ExpandCapture:
            evt.setFlags |= .Name | .Disabled
            evt.name     = "Expand Capture..."
            evt.disabled = !capture

        case TrimCapture:
            evt.setFlags |= .Name | .Disabled
            evt.name     = "Trim Capture"
            evt.disabled = !capture

        case RestoreOrg:
            evt.setFlags |= .Name | .Disabled
            evt.name     = "Restore to Original"
            evt.disabled = !capture or !capture.backImgOriginal.isValid()

        case FlipImage:
            evt.setFlags |= .Name | .Disabled
            evt.name     = "Flip Image"
            evt.disabled = !capture

        case MirrorImage:
            evt.setFlags |= .Name | .Disabled
            evt.name     = "Mirror Image"
            evt.disabled = !capture

        case RotateImageRight:
            evt.setFlags |= .Name | .Disabled
            evt.name     = "Rotate Image Right"
            evt.disabled = !capture

        case RotateImageLeft:
            evt.setFlags |= .Name | .Disabled
            evt.name     = "Rotate Image Left"
            evt.disabled = !capture
        }

        return true
    }

    mtd impl execute(cxt: ActionContext)->bool
    {
        let main    = cast(*MainWnd) cxt.wnd
        let capture = assume main.getCapture()

        if !capture.backImgOriginal.isValid() do
            capture.backImgOriginal = capture.backImg

        switch cxt.id
        {
        case ResizeImage:
            main.resizeImage()
        case ExpandImage:
            main.expandImage()

        case FlipImage:
            main.doProcessImage(FlipImage)
        case MirrorImage:
            main.doProcessImage(MirrorImage)

        case RotateImageRight:
            main.doProcessImage(RotateImageRight)
        case RotateImageLeft:
            main.doProcessImage(RotateImageLeft)

        case ExpandCapture:
            main.expandCapture()
        case TrimCapture:
            main.trimCapture()

        case RestoreOrg:
            let undo = capture.newUndo(.RestoreOrg)
            capture.pushUndo(undo)
            main.invalidate()
        }

        return true
    }
}

impl MainWnd
{
    mtd doProcessImage(cmdId: WndId, l, t, r, b, w, h: s32 = 0)
    {
        let capture = assume .getCapture()

        capture.serialUndo += 1
        let serial = capture.serialUndo

        var img  = capture.backImg
        let oldW = img.width
        let oldH = img.height

        switch cmdId
        {
        case ActionImage.ResizeImage:
            img.resize(w, h, .Bilinear)
        case ActionImage.ExpandImage:
            img.expand(l, t, r, b)
        case ActionImage.FlipImage:
            img.flip()
        case ActionImage.MirrorImage:
            img.mirror()
        case ActionImage.RotateImageRight:
            img.turn(.T90)
        case ActionImage.RotateImageLeft:
            img.turn(.T270)
        }

        let undo1 = capture.newUndo(.SetImage, serial)
        undo1.newImg = img
        capture.pushUndo(undo1)

        switch cmdId
        {
        case ActionImage.ResizeImage:
            let undo2 = capture.newUndo(.MoveCapture, serial)
            undo2.newImg   = img
            undo2.newSizeX = w - capture.width
            undo2.newSizeY = h - capture.height
            capture.pushUndo(undo2)

        case ActionImage.RotateImageRight, ActionImage.RotateImageLeft:
            let undo2 = capture.newUndo(.MoveCapture, serial)
            undo2.newImg   = img
            undo2.newSizeX = img.width - oldW
            undo2.newSizeY = img.height - oldH
            capture.pushUndo(undo2)
        }
    }

    mtd resizeImage()
    {
        let capture = assume .getCapture()
        let ratio   = cast(f32) capture.width / capture.height

        let dlg = Dialog.createDialog(.surface, 350, 265, title: "Resize Image")
        dlg.addButton("Resize", Dialog.BtnOk, true)
        dlg.addButton("Cancel", Dialog.BtnCancel)
        let edit0 = EditCtrl.create(dlg, "Width", Format.toString("%", capture.width), {80, 15}, flags: .RightAlign)
        let edit1 = EditCtrl.create(dlg, "Height", Format.toString("%", capture.height), {80, 50}, flags: .RightAlign)
        edit0.edit.setInputU64(cast() Capture.MinSize, cast() Capture.MaxSize)
        edit1.edit.setInputU64(cast() Capture.MinSize, cast() Capture.MaxSize)
        let btnRatio = CheckButton.create(dlg, "Keep Ratio", {120, 85})
        btnRatio.checked = g_KeepRatio ? .Checked : .UnChecked

        edit0.edit.sigChanged += func|ratio, btnRatio, edit1|(edt)
        {
            if btnRatio.checked == .Checked
            {
                let w = trycatch StrConv.toS32(edt.text)
                let h = Math.max(1, cast(s32) (w / ratio))
                edit1.edit.setTextSilent(Format.toString("%", h))
            }
        }
        edit1.edit.sigChanged += func|ratio, btnRatio, edit0|(edt)
        {
            if btnRatio.checked == .Checked
            {
                let h = trycatch StrConv.toS32(edt.text)
                let w = Math.max(1, cast(s32) (h * ratio))
                edit0.edit.setTextSilent(Format.toString("%", w))
            }
        }

        let res = dlg.doModal()
        g_KeepRatio = btnRatio.checked == .Checked
        if res != Dialog.BtnOk do
            return

        let w = trycatch StrConv.toS32(edit0.edit.text)
        if w < Capture.MinSize or w > Capture.MaxSize do
            return
        let h = trycatch StrConv.toS32(edit1.edit.text)
        if h < Capture.MinSize or h > Capture.MaxSize do
            return
        if w == capture.width and h == capture.height do
            return

        .doProcessImage(ActionImage.ResizeImage, w: w, h: h)
    }

    func createBtnGrid(dlg: *Wnd)
    {
        func createBtn(dlg: *Wnd, pos: Math.Rectangle, id: u64)
        {
            let btn = CheckButton.create(dlg, " ", pos)
            btn.checked  = g_ResizeMode == id ? .Undefined : .UnChecked
            btn.userData = cast(*void) id
            btn.sigChanged += func(btn)
            {
                foreach c in btn.parent.childs
                {
                    if c.userData
                    {
                        let btnc = cast(*CheckButton) c
                        btnc.checked = c != btn ? .UnChecked : .Undefined
                        btnc.invalidate()
                    }
                }
            }
        }

        const X = 175
        const Y = 15
        const S = 20
        const O = 4
        createBtn(dlg, {X, Y, S, S}, 1)
        createBtn(dlg, {X + S + O, Y, S, S}, 2)
        createBtn(dlg, {X + S + O + S + O, Y, S, S}, 3)
        createBtn(dlg, {X, Y + S + O, S, S}, 4)
        createBtn(dlg, {X + S + O, Y + S + O, S, S}, 5)
        createBtn(dlg, {X + S + O + S + O, Y + S + O, S, S}, 6)
        createBtn(dlg, {X, Y + S + O + S + O, S, S}, 7)
        createBtn(dlg, {X + S + O, Y + S + O + S + O, S, S}, 8)
        createBtn(dlg, {X + S + O + S + O, Y + S + O + S + O, S, S}, 9)
    }

    mtd createExpandDialog(title: string, w, h: s32)->WndId
    {
        let dlg = Dialog.createDialog(.surface, 350, 265, title: title)
        dlg.addButton("Resize", Dialog.BtnOk, true)
        dlg.addButton("Cancel", Dialog.BtnCancel)

        let ratio = cast(f32) w / h

        g_Width  = w
        g_Height = h
        let edit0 = EditCtrl.create(dlg, "Width", Format.toString("%", w), {30, 15}, flags: .RightAlign)
        let edit1 = EditCtrl.create(dlg, "Height", Format.toString("%", h), {30, 50}, flags: .RightAlign)
        edit0.edit.setInputU64(cast() Capture.MinSize, cast() Capture.MaxSize)
        edit1.edit.setInputU64(cast() Capture.MinSize, cast() Capture.MaxSize)

        edit0.edit.sigChanged += func|ratio, edit1|(edt)
        {
            g_Width = trycatch StrConv.toS32(edt.text)
            if g_KeepRatio
            {
                g_Height = Math.max(1, cast(s32) (g_Width / ratio))
                edit1.edit.setTextSilent(Format.toString("%", g_Height))
            }
        }

        edit1.edit.sigChanged += func|ratio, edit0|(edt)
        {
            g_Height = trycatch StrConv.toS32(edt.text)
            if g_KeepRatio
            {
                g_Width = Math.max(1, cast(s32) (g_Height * ratio))
                edit0.edit.setTextSilent(Format.toString("%", g_Width))
            }
        }

        let btnRatio = CheckButton.create(dlg, "Keep Ratio", {70, 85})
        btnRatio.checked = g_KeepRatio ? .Checked : .UnChecked
        btnRatio.sigChanged += func||(btn)
        {
            g_KeepRatio = btn.isChecked()
        }

        dlg.createBtnGrid()

        let res = dlg.doModal()

        foreach c in dlg.childs
        {
            if c.userData
            {
                let btnc = cast(*CheckButton) c
                if btnc.checked != .UnChecked
                {
                    g_ResizeMode = cast(u64) btnc.userData
                    break
                }
            }
        }

        return res
    }

    mtd expandImage()
    {
        let capture = assume .getCapture()
        let res     = .createExpandDialog("Expand Image", capture.backImg.width, capture.backImg.height)
        if res != Dialog.BtnOk do
            return

        let w = g_Width
        if w < Capture.MinSize or w > Capture.MaxSize do
            return
        let h = g_Height
        if h < Capture.MinSize or h > Capture.MaxSize do
            return
        if w == capture.backImg.width and h == capture.backImg.height do
            return

        var l, t, r, b: s32 = 0
        let cw = w - cast(s32) capture.backImg.width
        let ch = h - cast(s32) capture.backImg.height

        switch g_ResizeMode
        {
        case 1, 4, 7:
            r += cw
        case 2, 5, 8:
            l += cw / 2; r += cw / 2
        case 3, 6, 9:
            l += cw
        }

        switch g_ResizeMode
        {
        case 1, 2, 3:
            b += ch
        case 4, 5, 6:
            t += ch / 2; b += ch / 2
        case 7, 8, 9:
            t += ch
        }

        .doProcessImage(ActionImage.ExpandImage, l, t, r, b)
    }

    mtd expandCapture()
    {
        let capture = assume .getCapture()
        let res     = .createExpandDialog("Expand Capture", capture.width, capture.height)
        if res != Dialog.BtnOk do
            return

        let w = g_Width
        if w < Capture.MinSize or w > Capture.MaxSize do
            return
        let h = g_Height
        if h < Capture.MinSize or h > Capture.MaxSize do
            return
        if w == capture.width and h == capture.height do
            return

        let nw = w - capture.width
        let nh = h - capture.height

        let undo = capture.newUndo(.MoveCapture)
        undo.newSizeX = nw
        undo.newSizeY = nh

        switch g_ResizeMode
        {
        case 2, 5, 8:
            undo.newPosX = nw / 2
        case 3, 6, 9:
            undo.newPosX = nw
        }

        switch g_ResizeMode
        {
        case 4, 5, 6:
            undo.newPosY = nh / 2
        case 7, 8, 9:
            undo.newPosY = nh
        }

        capture.pushUndo(undo)
    }

    mtd trimCapture()
    {
        let capture = assume .getCapture()
        capture.selection.clear()

        var minX = Swag.F32.Max
        var minY = Swag.F32.Max
        var maxX = 0'f32
        var maxY = 0'f32

        foreach f, idx in capture.forms
        {
            let r = f.boundRect()
            if r.x > 0 do
                minX = Math.min(minX, r.x)
            if r.y > 0 do
                minY = Math.min(minY, r.y)
            if r.right() < capture.width do
                maxX = Math.max(maxX, r.right())
            if r.bottom() < capture.height do
                maxY = Math.max(maxY, r.bottom())

            // Select forms outside capture
            if r.right() < 0 or
               r.bottom() < 0 or
               r.x >= capture.width or
               r.y >= capture.height
            {
                capture.selection.add(idx)
            }
        }

        #scope(Left)
        for x in capture.backImg.width
        {
            if x + capture.backImagePos.x < 0 do
                continue
            for y in capture.backImg.height
            {
                let col = capture.backImg.getPixelColor(x, y)
                if col.a
                {
                    minX = Math.min(minX, x + capture.backImagePos.x)
                    break to Left
                }
            }
        }

        #scope(Right)
        for #reverse x in capture.backImg.width
        {
            if x + capture.backImagePos.x >= capture.width do
                continue
            for y in capture.backImg.height
            {
                let col = capture.backImg.getPixelColor(x, y)
                if col.a
                {
                    maxX = Math.max(maxX, x + capture.backImagePos.x + 1)
                    break to Right
                }
            }
        }

        #scope(Up)
        for y in capture.backImg.height
        {
            if y + capture.backImagePos.y < 0 do
                continue
            for x in capture.backImg.width
            {
                let col = capture.backImg.getPixelColor(x, y)
                if col.a
                {
                    minY = Math.min(minY, y + capture.backImagePos.y)
                    break to Up
                }
            }
        }

        #scope(Down)
        for #reverse y in capture.backImg.height
        {
            if y + capture.backImagePos.y >= capture.height do
                continue
            for x in capture.backImg.width
            {
                let col = capture.backImg.getPixelColor(x, y)
                if col.a
                {
                    maxY = Math.max(maxY, y + capture.backImagePos.y + 1)
                    break to Down
                }
            }
        }

        capture.serialUndo += 1
        let serial = capture.serialUndo

        // Resize capture
        if minX or minY or maxX != capture.width or maxY != capture.height
        {
            with let undo = capture.newUndo(.MoveCapture, serial)
            {
                .newPosX -= cast(s32) minX
                .newSizeX -= cast(s32) minX
                .newPosY -= cast(s32) minY
                .newSizeY -= cast(s32) minY
                .newSizeX -= cast(s32) (capture.width - maxX)
                .newSizeY -= cast(s32) (capture.height - maxY)
            }
            capture.pushUndo(undo)
        }

        // Remove forms outside the cavenas
        if capture.selection.count
        {
            let undo = capture.newUndo(.DeleteSelection, serial)
            capture.pushUndo(undo)
        }

        // Crop image
        if capture.backImagePos.x < 0 or
           capture.backImagePos.y < 0 or
           capture.backImagePos.x + capture.backImg.width > capture.width or
           capture.backImagePos.y + capture.backImg.height > capture.height
        {
            var cropRect = Math.Rectangle{0, 0, capture.backImg.width, capture.backImg.height}

            let dx = Math.max(0, -capture.backImagePos.x)
            let dy = Math.max(0, -capture.backImagePos.y)
            let dw = Math.max(0, (capture.backImagePos.x + capture.backImg.width) - capture.width)
            let dh = Math.max(0, (capture.backImagePos.y + capture.backImg.height) - capture.height)
            cropRect.moveLeft(dx)
            cropRect.moveTop(dy)
            cropRect.width -= dw
            cropRect.height -= dh

            let undo = capture.newUndo(.SetImage, serial)
            undo.newImg = capture.backImg.toCrop(cropRect)
            capture.pushUndo(undo)
        }

        capture.selectionHasChanged()
    }
}
