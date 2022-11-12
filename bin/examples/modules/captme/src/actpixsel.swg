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
            boundRects.add(f.boundRect(false))

        params := @{capture, boundRects: boundRects.buffer, forms: &forms}
        undo.newImg.visitPixels(&params)
        {
            p := cast(*@typeof(params)) userData
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
            boundRects.add(f.boundRect(false))

        params := @{capture, boundRects: boundRects.buffer, forms: &forms}
        undo.newImg.visitPixels(&params)
        {
            p := cast(*@typeof(params)) userData
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

    mtd pixSelSmartFill()
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
            boundRects.add(f.boundRect(false))

        params := @{capture, boundRects: boundRects.buffer, forms: &forms}
        undo.newImg.visitPixels(&params)
        {
            p := cast(*@typeof(params)) userData
            visit f, idx: dref p.forms
            {
                pt := Math.Point{cast(f32) x, cast(f32) p.capture.backImg.height - y}
                pt.x += p.capture.backImagePos.x
                pt.y += p.capture.backImagePos.y
                if !p.boundRects[idx].contains(pt)
                    continue

                if f.isInside(pt)
                {
                    var col1, col2: Color

                    x1 := x
                    while x1 > 0
                    {
                        x1 -= 1
                        pt.x = x1 + p.capture.backImagePos.x
                        if !p.boundRects[idx].contains(pt) or !f.isInside(pt)
                        {
                            pix32 := image.pixels.buffer + x1 * image.bpp8
                            pix32 += y * image.width * image.bpp8
                            col1.argb = dref cast(*u32) pix32
                            break
                        }
                    }

                    x2 := x
                    while x2 > 0
                    {
                        x2 += 1
                        pt.x = x2 + p.capture.backImagePos.x
                        if !p.boundRects[idx].contains(pt) or !f.isInside(pt)
                        {
                            pix32 := image.pixels.buffer + x2 * image.bpp8
                            pix32 += y * image.width * image.bpp8
                            col2.argb = dref cast(*u32) pix32
                            break
                        }
                    }

                    fHorz := (x - x1) / cast(f32) (x2 - x1)
                    col := Color.getBlend(col1, col2, fHorz)

                    dref cast(*u32) pix = col.argb
                    break
                }
            }
        }

        capture.pushUndo(undo)
    }
}