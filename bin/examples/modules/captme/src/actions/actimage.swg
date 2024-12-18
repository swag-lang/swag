const CmdIdTrimCapture      = "CmdIdTrimCapture"
const CmdIdResizeImage      = "CmdIdResizeImage"
const CmdIdExpandImage      = "CmdIdExpandImage"
const CmdIdExpandCapture    = "CmdIdExpandCapture"
const CmdIdRestoreOrg       = "CmdIdRestoreOrg"
const CmdIdFlipImage        = "CmdIdFlipImage"
const CmdIdMirrorImage      = "CmdIdMirrorImage"
const CmdIdRotateImageRight = "CmdIdRotateImageRight"
const CmdIdRotateImageLeft  = "CmdIdRotateImageLeft"

struct ActionImage {}
impl IActionUI for ActionImage
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case CmdIdTrimCapture, CmdIdResizeImage, CmdIdExpandImage,
             CmdIdExpandCapture, CmdIdRestoreOrg,
             CmdIdFlipImage, CmdIdMirrorImage, 
             CmdIdRotateImageRight, CmdIdRotateImageLeft:
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
        case CmdIdResizeImage:
            evt.setFlags |= .Name | .Disabled
            evt.name     = "Resize Image..."
            evt.disabled = !capture

        case CmdIdExpandImage:
            evt.setFlags |= .Name | .Disabled
            evt.name     = "Expand Image..."
            evt.disabled = !capture            

        case CmdIdExpandCapture:
            evt.setFlags |= .Name | .Disabled
            evt.name     = "Expand Capture..."
            evt.disabled = !capture

        case CmdIdTrimCapture:
            evt.setFlags |= .Name | .Disabled
            evt.name     = "Trim Capture"
            evt.disabled = !capture

        case CmdIdRestoreOrg:
            evt.setFlags |= .Name | .Disabled
            evt.name     = "Restore to Original"
            evt.disabled = !capture or !capture.backImgOrg.isValid()

        case CmdIdFlipImage:
            evt.setFlags |= .Name | .Disabled
            evt.name     = "Flip Image"
            evt.disabled = !capture

        case CmdIdMirrorImage:
            evt.setFlags |= .Name | .Disabled
            evt.name     = "Mirror Image"
            evt.disabled = !capture

        case CmdIdRotateImageRight:
            evt.setFlags |= .Name | .Disabled
            evt.name     = "Rotate Image Right"
            evt.disabled = !capture

        case CmdIdRotateImageLeft:
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

        switch cxt.id
        {
        case CmdIdResizeImage:
            main.resizeImage()
        case CmdIdExpandImage:
            main.expandImage()            

        case CmdIdFlipImage:
            main.doProcessImage(CmdIdFlipImage)
        case CmdIdMirrorImage:
            main.doProcessImage(CmdIdMirrorImage)

        case CmdIdRotateImageRight:
            main.doProcessImage(CmdIdRotateImageRight)            
        case CmdIdRotateImageLeft:
            main.doProcessImage(CmdIdRotateImageLeft)                  

        case CmdIdExpandCapture:
            main.expandCapture()    
        case CmdIdTrimCapture:
            main.trimCapture()

        case CmdIdRestoreOrg:
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
        let capture = assume getCapture()

        capture.serialUndo += 1
        let serial = capture.serialUndo

        var img = capture.backImg
        let oldW = img.width
        let oldH = img.height

        switch cmdId
        {
        case CmdIdResizeImage:
            img.resize(w, h, .Bilinear)
        case CmdIdExpandImage:
            img.expand(l, t, r, b)            
        case CmdIdFlipImage:
            img.flip()
        case CmdIdMirrorImage:
            img.mirror()
        case CmdIdRotateImageRight:
            img.turn(.T90)
        case CmdIdRotateImageLeft:
            img.turn(.T270)            
        }

        let undo1 = capture.newUndo(.SetImage, serial)
        undo1.newImg = img
        capture.pushUndo(undo1)

        switch cmdId
        {
        case CmdIdResizeImage:
            let undo2 = capture.newUndo(.MoveCapture, serial)
            undo2.newImg   = img
            undo2.newSizeX = w - capture.width
            undo2.newSizeY = h - capture.height
            capture.pushUndo(undo2)

        case CmdIdRotateImageRight, CmdIdRotateImageLeft:
            let undo2 = capture.newUndo(.MoveCapture, serial)
            undo2.newImg   = img
            undo2.newSizeX = img.width - oldW
            undo2.newSizeY = img.height - oldH
            capture.pushUndo(undo2)        
        }
    }

    mtd resizeImage()
    {
        #[Swag.Global]
        var g_KeepRatio: bool = true

        let capture = assume getCapture()
        let ratio   = cast(f32) capture.width / capture.height

        let dlg = Dialog.createDialog(surface, 350, 265, title: "Resize Image")
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
        if res != Dialog.BtnOk:
            return

        let w = trycatch StrConv.toS32(edit0.edit.text)
        if w < Capture.MinSize or w > Capture.MaxSize:
            return
        let h = trycatch StrConv.toS32(edit1.edit.text)
        if h < Capture.MinSize or h > Capture.MaxSize:
            return
        if w == capture.width and h == capture.height:
            return

        doProcessImage(CmdIdResizeImage, w: w, h: h)
    }  

    mtd expandImage()
    {
        #[Swag.Global]
        var g_KeepRatio: bool = true
        #[Swag.Global]
        var g_ResizeMode: u64 = 5

        let capture = assume getCapture()
        let ratio   = cast(f32) capture.width / capture.height

        let dlg = Dialog.createDialog(surface, 350, 265, title: "Expand Image")
        dlg.addButton("Resize", Dialog.BtnOk, true)
        dlg.addButton("Cancel", Dialog.BtnCancel)
        let edit0 = EditCtrl.create(dlg, "Width", Format.toString("%", capture.backImg.width), {30, 15}, flags: .RightAlign)
        let edit1 = EditCtrl.create(dlg, "Height", Format.toString("%", capture.backImg.height), {30, 50}, flags: .RightAlign)
        edit0.edit.setInputU64(cast() Capture.MinSize, cast() Capture.MaxSize)
        edit1.edit.setInputU64(cast() Capture.MinSize, cast() Capture.MaxSize)
        let btnRatio = CheckButton.create(dlg, "Keep Ratio", {70, 85})
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

        let res = dlg.doModal()

        g_KeepRatio = btnRatio.checked == .Checked
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

        if res != Dialog.BtnOk:
            return

        let w = trycatch StrConv.toS32(edit0.edit.text)
        if w < Capture.MinSize or w > Capture.MaxSize:
            return
        let h = trycatch StrConv.toS32(edit1.edit.text)
        if h < Capture.MinSize or h > Capture.MaxSize:
            return
        if w == capture.backImg.width and h == capture.backImg.height:
            return

        var l, t, r, b: s32 = 0
        let cw = w - cast(s32) capture.backImg.width
        let ch = h - cast(s32) capture.backImg.height

        switch g_ResizeMode
        {
        case 1, 4, 7: r += cw
        case 2, 5, 8: l += cw / 2; r += cw / 2
        case 3, 6, 9: l += cw
        }

        switch g_ResizeMode
        {
        case 1, 2, 3: b += ch
        case 4, 5, 6: t += ch / 2; b += ch / 2
        case 7, 8, 9: t += ch
        }        

        doProcessImage(CmdIdExpandImage, l, t, r, b)
    }

    mtd expandCapture()
    {
        #[Swag.Global]
        var g_KeepRatio: bool = true
        #[Swag.Global]
        var g_ResizeMode: u64 = 5

        let capture = assume getCapture()
        let ratio   = cast(f32) capture.width / capture.height

        let dlg = Dialog.createDialog(surface, 350, 265, title: "Expand Capture")
        dlg.addButton("Resize", Dialog.BtnOk, true)
        dlg.addButton("Cancel", Dialog.BtnCancel)
        let edit0 = EditCtrl.create(dlg, "Width", Format.toString("%", capture.width), {30, 15}, flags: .RightAlign)
        let edit1 = EditCtrl.create(dlg, "Height", Format.toString("%", capture.height), {30, 50}, flags: .RightAlign)
        edit0.edit.setInputU64(cast() Capture.MinSize, cast() Capture.MaxSize)
        edit1.edit.setInputU64(cast() Capture.MinSize, cast() Capture.MaxSize)
        let btnRatio = CheckButton.create(dlg, "Keep Ratio", {70, 85})
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

        let res = dlg.doModal()

        g_KeepRatio = btnRatio.checked == .Checked
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

        if res != Dialog.BtnOk:
            return

        let w = trycatch StrConv.toS32(edit0.edit.text)
        if w < Capture.MinSize or w > Capture.MaxSize:
            return
        let h = trycatch StrConv.toS32(edit1.edit.text)
        if h < Capture.MinSize or h > Capture.MaxSize:
            return
        if w == capture.width and h == capture.height:
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
        let capture = assume getCapture()
        capture.selection.clear()

        var minX = Swag.F32.Max
        var minY = Swag.F32.Max
        var maxX = 0'f32
        var maxY = 0'f32

        foreach f, idx in capture.forms
        {
            var r = f.boundRect()
            if r.x > 0:
                minX = Math.min(minX, r.x)
            if r.y > 0:
                minY = Math.min(minY, r.y)
            if r.right() < capture.width:
                maxX = Math.max(maxX, r.right())
            if r.bottom() < capture.height:
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

        #scope Left
        for x in capture.backImg.width
        {
            if x + capture.backImagePos.x < 0:
                continue
            for y in capture.backImg.height
            {
                var col = capture.backImg.getPixelColor(x, y)
                if col.a
                {
                    minX = Math.min(minX, x + capture.backImagePos.x)
                    break to Left
                }
            }
        }

        #scope Right
        for #back x in capture.backImg.width
        {
            if x + capture.backImagePos.x >= capture.width:
                continue
            for y in capture.backImg.height
            {
                var col = capture.backImg.getPixelColor(x, y)
                if col.a
                {
                    maxX = Math.max(maxX, x + capture.backImagePos.x + 1)
                    break to Right
                }
            }
        }

        #scope Up
        for y in capture.backImg.height
        {
            if y + capture.backImagePos.y < 0:
                continue
            for x in capture.backImg.width
            {
                var col = capture.backImg.getPixelColor(x, y)
                if col.a
                {
                    minY = Math.min(minY, y + capture.backImagePos.y)
                    break to Up
                }
            }
        }

        #scope Down
        for #back y in capture.backImg.height
        {
            if y + capture.backImagePos.y >= capture.height:
                continue
            for x in capture.backImg.width
            {
                var col = capture.backImg.getPixelColor(x, y)
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
            let undo = capture.newUndo(.MoveCapture, serial)
            undo.newPosX -= cast(s32) minX
            undo.newSizeX -= cast(s32) minX
            undo.newPosY -= cast(s32) minY
            undo.newSizeY -= cast(s32) minY
            undo.newSizeX -= cast(s32) (capture.width - maxX)
            undo.newSizeY -= cast(s32) (capture.height - maxY)
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
            if capture.backImagePos.x < 0:
                cropRect.moveLeft(-capture.backImagePos.x)
            if capture.backImagePos.y < 0:
                cropRect.moveTop(-capture.backImagePos.y)
            if capture.backImagePos.x + capture.backImg.width > capture.width:
                cropRect.width -= (capture.backImagePos.x + capture.backImg.width) - capture.width
            if capture.backImagePos.y + capture.backImg.height > capture.height:
                cropRect.height -= (capture.backImagePos.y + capture.backImg.height) - capture.height

            let undo = capture.newUndo(.SetImage, serial)
            undo.newImg = capture.backImg.toCrop(cropRect)
            capture.pushUndo(undo)
        }

        capture.selectionHasChanged()
    }
}
