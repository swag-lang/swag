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

                    y1 := cast(s32) (p.boundRects[idx].bottom() - p.capture.backImagePos.y)
                    y1 = image.height - y1
                    loop i: image.height
                    {
                        y1 += 1
                        pt.y = cast(f32) (p.capture.backImg.height - y1) + p.capture.backImagePos.y
                        if p.boundRects[idx].contains(pt) and f.isInside(pt)
                        {
                            pix32 := image.pixels.buffer + x * image.bpp8
                            pix32 += y1 * image.width * image.bpp8
                            col1.argb = dref cast(*u32) pix32
                            break
                        }
                    }

                    y2 := cast(s32) (p.boundRects[idx].y - p.capture.backImagePos.y)
                    y2 = image.height - y2
                    loop i: image.height
                    {
                        y2 -= 1
                        pt.y = cast(f32) (p.capture.backImg.height - y2) + p.capture.backImagePos.y
                        if p.boundRects[idx].contains(pt) and f.isInside(pt)
                        {
                            pix32 := image.pixels.buffer + x * image.bpp8
                            pix32 += y2 * image.width * image.bpp8
                            col2.argb = dref cast(*u32) pix32
                            break
                        }
                    }

                    fVert := (y - y1) / cast(f32) (y2 - y1)
                    fVert = Math.clamp(fVert, 0, 1)
                    col := Color.getBlend(col1, col2, fVert)

                    dref cast(*u32) pix = col.argb
                    break
                }
            }
        }

        capture.pushUndo(undo)
    }
}