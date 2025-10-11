struct ActionPixSel {}

impl ActionPixSel
{
    newCmdId("Clear")
    newCmdId("Fill")
    newCmdId("SmartFill")
    newCmdId("SmartMove")
}

impl IActionUI for ActionPixSel
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case Clear, Fill, SmartFill, SmartMove:
            return true
        }

        return false
    }

    mtd impl update(cxt: ActionContext, evt: *CommandStateEvent)->bool
    {
        let main    = cast(*MainWnd) cxt.wnd
        let capture = main.getCapture()

        var hasPixSel = false
        if capture
        {
            foreach s in capture.selection
            {
                let f = capture.forms[s]
                let b = cast(*BaseForm) @dataof(f)
                hasPixSel = b.subKind == .PixSel
                if hasPixSel do
                    break
            }
        }

        switch cxt.id
        {
        case Clear:
            evt.setFlags |= .Name | .Disabled | .Hidden
            evt.name     = "Clear"
            evt.disabled = !capture or !hasPixSel
            evt.hidden   = evt.disabled

        case Fill:
            evt.setFlags |= .Name | .Disabled | .Hidden
            evt.name     = "Fill"
            evt.disabled = !capture or !hasPixSel
            evt.hidden   = evt.disabled

        case SmartFill:
            evt.setFlags |= .Name | .Disabled | .Hidden
            evt.name     = "Smart Fill"
            evt.disabled = !capture or !hasPixSel
            evt.hidden   = evt.disabled

        case SmartMove:
            evt.setFlags |= .Name | .Disabled | .Hidden
            evt.name     = "Smart Move"
            evt.disabled = !capture or !hasPixSel
            evt.hidden   = evt.disabled
        }

        return true
    }

    mtd impl execute(cxt: ActionContext)->bool
    {
        let main = cast(*MainWnd) cxt.wnd

        switch cxt.id
        {
        case Clear:
            main.pixSelClear()
        case Fill:
            main.pixSelFill()
        case SmartFill:
            main.pixSelSmartFill()
        case SmartMove:
            main.pixSelSmartMove()
        }

        return true
    }
}

impl MainWnd
{
    mtd getPixSels()->Array'IForm
    {
        var result: retval
        let capture = assume .getCapture()
        foreach s in capture.selection
        {
            let f = capture.forms[s]
            let b = cast(*BaseForm) @dataof(f)
            if b.subKind == .PixSel do
                result.add(f)
        }

        return result
    }

    mtd pixSelClear()
    {
        var forms = .getPixSels()
        if forms.count == 0 do
            return

        let capture = assume .getCapture()
        let undo    = capture.newUndo(.SetImage)
        undo.newImagePos = capture.backImagePos
        undo.newImg      = capture.backImg

        var boundRects: Array'Math.Rectangle
        foreach f in forms do
            boundRects.add(f.boundRect())

        var params = {capture, boundRects: boundRects.buffer, forms: &forms}
        undo.newImg.visitPixels(&params)
        {
            var p = cast(*#decltype(params)) userData
            foreach f, idx in dref p.forms
            {
                var pt = Math.Point{cast(f32) x, cast(f32) p.capture.backImg.height - y}
                pt.x += p.capture.backImagePos.x
                pt.y += p.capture.backImagePos.y
                if !p.boundRects[idx].contains(pt) do
                    continue

                if f.isInside(pt)
                {
                    dref cast(*u32) pix = 0
                    break
                }
            }
        }

        capture.pushUndo(undo)
    }

    mtd pixSelFill()
    {
        var forms = .getPixSels()
        if forms.count == 0 do
            return

        let capture = assume .getCapture()
        let undo    = capture.newUndo(.SetImage)
        undo.newImagePos = capture.backImagePos
        undo.newImg      = capture.backImg

        var boundRects: Array'Math.Rectangle
        foreach f in forms do
            boundRects.add(f.boundRect())

        var params = {capture, boundRects: boundRects.buffer, forms: &forms}
        undo.newImg.visitPixels(&params)
        {
            var p = cast(*#decltype(params)) userData
            foreach f, idx in dref p.forms
            {
                var pt = Math.Point{cast(f32) x, cast(f32) p.capture.backImg.height - y}
                pt.x += p.capture.backImagePos.x
                pt.y += p.capture.backImagePos.y
                if !p.boundRects[idx].contains(pt) do
                    continue

                if f.isInside(pt)
                {
                    dref cast(*u32) pix = g_Main.pixSelFillCol.argb
                    break
                }
            }
        }

        capture.pushUndo(undo)
    }

    mtd pixSelSmartFill(serial: u32 = 0, masks: *Array'Image = null)
    {
        var forms = .getPixSels()
        if forms.count == 0 do
            return

        let capture = assume .getCapture()
        let undo    = capture.newUndo(.SetImage, serial)
        undo.newImagePos = capture.backImagePos
        undo.newImg      = capture.backImg

        struct Info
        {
            boundRect:     Math.Rectangle
            boundCols:     Array'{ y1: s32, y2: s32, col1: Color, col2: Color }
        }

        var infos: Array'Info

        foreach f in forms
        {
            var i: Info

            let br = f.boundRect()
            i.boundRect = br

            if masks
            {
                var img = Image.create(1 + cast(s32) br.width, 1 + cast(s32) br.height, .RGBA8)
                masks.add(moveref &img)
            }

            // Compute colors at the start and at the end of the column, to interpolate
            for x in undo.newImg.width
            {
                var col1, col2: Color
                var pt = Math.Point{cast(f32) x, 0}
                var y1 = cast(s32) (br.bottom() - capture.backImagePos.y)
                y1 = Math.clamp(y1, 0, undo.newImg.height - 1)
                y1 = undo.newImg.height - y1
                for i in undo.newImg.height
                {
                    pt.y = cast(f32) (capture.backImg.height - y1) + capture.backImagePos.y
                    if br.contains(pt) and f.isInside(pt)
                    {
                        var pix32 = undo.newImg.pixels.buffer + x * undo.newImg.bpp8
                        pix32 += y1 * undo.newImg.width * undo.newImg.bpp8
                        col1.argb = dref cast(*u32) pix32
                        break
                    }
                    y1 += 1
                }

                var y2 = cast(s32) (br.y - capture.backImagePos.y)
                y2 = undo.newImg.height - y2
                y2 = Math.clamp(y2, 0, undo.newImg.height - 1)
                for i in undo.newImg.height
                {
                    pt.y = cast(f32) (capture.backImg.height - y2) + capture.backImagePos.y
                    if br.contains(pt) and f.isInside(pt)
                    {
                        var pix32 = undo.newImg.pixels.buffer + x * undo.newImg.bpp8
                        pix32 += y2 * undo.newImg.width * undo.newImg.bpp8
                        col2.argb = dref cast(*u32) pix32
                        break
                    }
                    y2 -= 1
                }

                i.boundCols.add({y1, y2, col1, col2})
            }

            infos.add(moveref &i)
        }

        var params = {capture, infos: infos.buffer, forms: &forms, masks}
        undo.newImg.visitPixels(&params)
        {
            var p = cast(*#decltype(params)) userData
            foreach f, idx in dref p.forms
            {
                var pt = Math.Point{cast(f32) x, cast(f32) p.capture.backImg.height - y}
                pt.x += p.capture.backImagePos.x
                pt.y += p.capture.backImagePos.y
                if !p.infos[idx].boundRect.contains(pt) do
                    continue

                if f.isInside(pt)
                {
                    let p1   = p.infos[idx].boundCols[x]
                    let col1 = p1.col1
                    let col2 = p1.col2
                    let y1   = p1.y1
                    let y2   = p1.y2
                    if y2 != y1
                    {
                        // Store pixel mask
                        if p.masks
                        {
                            let xx    = cast(s32) (pt.x - p.infos[idx].boundRect.x)
                            var yy    = cast(s32) (pt.y - p.infos[idx].boundRect.y)
                            let pmask = p.masks.buffer + idx
                            yy = pmask.height - yy
                            if xx < pmask.width and yy < pmask.height
                            {
                                var pix1 = pmask.pixels.buffer + xx * pmask.bpp8
                                pix1 += yy * pmask.width * pmask.bpp8
                                dref cast(*u32) pix1 = 0xFFFFFFFF
                            }
                        }

                        var fVert = (y - y1) / cast(f32) (y2 - y1)
                        fVert = Math.clamp(fVert, 0, 1)
                        let col = Color.getBlend(col1, col2, fVert)
                        dref cast(*u32) pix = col.argb
                    }

                    break
                }
            }
        }

        capture.pushUndo(undo)
    }

    mtd pixSelSmartMove()
    {
        let forms = .getPixSels()
        if forms.count == 0 do
            return

        let capture = assume .getCapture()

        capture.serialUndo += 1
        let serial = capture.serialUndo

        var undo = capture.newUndo(.CreateForm, serial)

        let startCount = capture.forms.count - forms.count
        foreach f in forms
        {
            let br = f.boundRect()

            with let nf = Memory.new'FormImage()
            {
                .kind             = .Capture
                .x                = cast(s32) br.x
                .y                = cast(s32) br.y
                .w                = cast(s32) br.width
                .h                = cast(s32) br.height
                .borderColor      = Argb.Zero
                .borderSize       = 0
                .paintShadow      = false
                .isReadyToCapture = true
                .IForm.setup(capture)
                .IForm.backCapture()
            }

            undo.oldForms.add(cast(IForm) nf)
        }

        capture.pushUndo(undo)

        var masks: Array'Image
        .pixSelSmartFill(serial, &masks)

        foreach f, idx in undo.oldForms
        {
            let nf = cast(*FormImage) f
            nf.IForm.backCaptureMask(masks.buffer + idx)
        }

        undo = capture.newUndo(.DeleteSelection, serial)
        capture.pushUndo(undo)

        capture.selection.clear()
        for i in startCount until capture.forms.count do
            capture.selection.add(i)
    }
}
