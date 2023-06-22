const CmdIdPixSelClear      = "CmdIdPixSelClear"
const CmdIdPixSelFill       = "CmdIdPixSelFill"
const CmdIdPixSelSmartFill  = "CmdIdPixSelSmartFill"
const CmdIdPixSelSmartMove  = "CmdIdPixSelSmartMove"

struct ActionPixSel {}
impl IActionUI for ActionPixSel
{
    mtd accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case CmdIdPixSelClear,
             CmdIdPixSelFill,
             CmdIdPixSelSmartFill,
             CmdIdPixSelSmartMove:
             return true
        }

        return false
    }

    mtd update(cxt: ActionContext, evt: *CommandStateEvent)->bool
    {
        main := cast(*MainWnd) cxt.wnd
        capture := main.getCapture()

        hasPixSel := false
        if capture
        {
            visit s: capture.selection
            {
                f := capture.forms[s]
                b := cast(*BaseForm) @dataof(f)
                hasPixSel = b.subKind == .PixSel
                if hasPixSel break
            }
        }

        switch cxt.id
        {
        case CmdIdPixSelClear:
            evt.setFlags |= .Name | .Disabled | .Hidden
            evt.name = "Clear"
            evt.disabled = !capture or !hasPixSel
            evt.hidden = evt.disabled

        case CmdIdPixSelFill:
            evt.setFlags |= .Name | .Disabled | .Hidden
            evt.name = "Fill"
            evt.disabled = !capture or !hasPixSel
            evt.hidden = evt.disabled

        case CmdIdPixSelSmartFill:
            evt.setFlags |= .Name | .Disabled | .Hidden
            evt.name = "Smart Fill"
            evt.disabled = !capture or !hasPixSel
            evt.hidden = evt.disabled

        case CmdIdPixSelSmartMove:
            evt.setFlags |= .Name | .Disabled | .Hidden
            evt.name = "Smart Move"
            evt.disabled = !capture or !hasPixSel
            evt.hidden = evt.disabled
        }

        return true
    }

    mtd execute(cxt: ActionContext)->bool
    {
        main := cast(*MainWnd) cxt.wnd

        switch cxt.id
        {
        case CmdIdPixSelClear:
            main.pixSelClear()
        case CmdIdPixSelFill:
            main.pixSelFill()
        case CmdIdPixSelSmartFill:
            main.pixSelSmartFill()
        case CmdIdPixSelSmartMove:
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
        capture := getCapture()
        visit s: capture.selection
        {
            f := capture.forms[s]
            b := cast(*BaseForm) @dataof(f)
            if b.subKind == .PixSel
                result.add(f)
        }

        return result
    }

    mtd pixSelClear()
    {
        forms := getPixSels()
        if forms.count == 0
            return

        capture := getCapture()
        undo := capture.newUndo(.SetImage)
        undo.newImagePos = capture.backImagePos
        undo.newImg = capture.backImg

        var boundRects: Array'Math.Rectangle
        visit f: forms
            boundRects.add(f.boundRect())

        params := {capture, boundRects: boundRects.buffer, forms: &forms}
        undo.newImg.visitPixels(&params)
        {
            p := cast(*@decltype(params)) userData
            visit f, idx: dref p.forms
            {
                pt := Math.Point{cast(f32) x, cast(f32) p.capture.backImg.height - y}
                pt.x += p.capture.backImagePos.x
                pt.y += p.capture.backImagePos.y
                if !p.boundRects[idx].contains(pt)
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
        forms := getPixSels()
        if forms.count == 0
            return

        capture := getCapture()
        undo := capture.newUndo(.SetImage)
        undo.newImagePos = capture.backImagePos
        undo.newImg = capture.backImg

        var boundRects: Array'Math.Rectangle
        visit f: forms
            boundRects.add(f.boundRect())

        params := {capture, boundRects: boundRects.buffer, forms: &forms}
        undo.newImg.visitPixels(&params)
        {
            p := cast(*@decltype(params)) userData
            visit f, idx: dref p.forms
            {
                pt := Math.Point{cast(f32) x, cast(f32) p.capture.backImg.height - y}
                pt.x += p.capture.backImagePos.x
                pt.y += p.capture.backImagePos.y
                if !p.boundRects[idx].contains(pt)
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
        forms := getPixSels()
        if forms.count == 0
            return

        capture := getCapture()
        undo := capture.newUndo(.SetImage, serial)
        undo.newImagePos = capture.backImagePos
        undo.newImg = capture.backImg

        struct Info
        {
            boundRect: Math.Rectangle
            boundCols: Array'{y1: s32, y2: s32, col1: Color, col2: Color}
        }

        var infos: Array'Info

        visit f: forms
        {
            var i: Info

            br := f.boundRect()
            i.boundRect = br

            if masks
            {
                img := Image.create(1 + cast(s32) br.width, 1 + cast(s32) br.height, .RGBA8)
                masks.add(moveref &img)
            }

            // Compute colors at the start and at the end of the column, to interpolate
            loop x: undo.newImg.width
            {
                var col1, col2: Color
                pt := Math.Point{cast(f32) x, 0}
                y1 := cast(s32) (br.bottom() - capture.backImagePos.y)
                y1 = undo.newImg.height - y1
                loop i: undo.newImg.height
                {
                    pt.y = cast(f32) (capture.backImg.height - y1) + capture.backImagePos.y
                    if br.contains(pt) and f.isInside(pt)
                    {
                        pix32 := undo.newImg.pixels.buffer + x * undo.newImg.bpp8
                        pix32 += y1 * undo.newImg.width * undo.newImg.bpp8
                        col1.argb = dref cast(*u32) pix32
                        break
                    }
                    y1 += 1
                }

                y2 := cast(s32) (br.y - capture.backImagePos.y)
                y2 = undo.newImg.height - y2
                loop i: undo.newImg.height
                {
                    pt.y = cast(f32) (capture.backImg.height - y2) + capture.backImagePos.y
                    if br.contains(pt) and f.isInside(pt)
                    {
                        pix32 := undo.newImg.pixels.buffer + x * undo.newImg.bpp8
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

        params := {capture, infos: infos.buffer, forms: &forms, masks}
        undo.newImg.visitPixels(&params)
        {
            p := cast(*@decltype(params)) userData
            visit f, idx: dref p.forms
            {
                pt := Math.Point{cast(f32) x, cast(f32) p.capture.backImg.height - y}
                pt.x += p.capture.backImagePos.x
                pt.y += p.capture.backImagePos.y
                if !p.infos[idx].boundRect.contains(pt)
                    continue

                if f.isInside(pt)
                {
                    p1   := p.infos[idx].boundCols[x]
                    col1 := p1.col1
                    col2 := p1.col2
                    y1   := p1.y1
                    y2   := p1.y2
                    if y2 != y1
                    {
                        // Store pixel mask
                        if p.masks
                        {
                            xx := cast(s32) (pt.x - p.infos[idx].boundRect.x)
                            yy := cast(s32) (pt.y - p.infos[idx].boundRect.y)
                            pmask := p.masks.buffer + idx
                            yy = pmask.height - yy
                            if xx < pmask.width and yy < pmask.height
                            {
                                pix1 := pmask.pixels.buffer + xx * pmask.bpp8
                                pix1 += yy * pmask.width * pmask.bpp8
                                dref cast(*u32) pix1 = 0xFFFFFFFF
                            }
                        }

                        fVert := (y - y1) / cast(f32) (y2 - y1)
                        fVert = Math.clamp(fVert, 0, 1)
                        col := Color.getBlend(col1, col2, fVert)
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
        forms := getPixSels()
        if forms.count == 0
            return

        capture := getCapture()

        capture.serialUndo += 1
        serial := capture.serialUndo

        undo := capture.newUndo(.CreateForm, serial)

        startCount := capture.forms.count - forms.count
        visit f: forms
        {
            br := f.boundRect()

            with nf := Memory.new'FormImage()
            {
                .kind = .Capture
                .x = cast(s32) br.x
                .y = cast(s32) br.y
                .w = cast(s32) br.width
                .h = cast(s32) br.height
                .borderColor = Argb.Zero
                .borderSize = 0
                .paintShadow = false
                .isReadyToCapture = true
                .IForm.setup(capture)
                .IForm.backCapture(getApp())
            }

            undo.oldForms.add(cast(IForm) nf)
        }

        capture.pushUndo(undo)

        var masks: Array'Image
        pixSelSmartFill(serial, &masks)

        visit f, idx: undo.oldForms
        {
            nf := cast(*FormImage) f
            nf.IForm.backCaptureMask(getApp(), masks.buffer + idx)
        }

        undo = capture.newUndo(.DeleteSelection, serial)
        capture.pushUndo(undo)

        capture.selection.clear()
        for i := startCount; i < capture.forms.count; i += 1
            capture.selection.add(i)
    }
}