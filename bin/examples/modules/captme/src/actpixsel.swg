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
        var boundCols:  Array'{y1: s32, y2: s32, col1: Color, col2: Color}

        visit f: forms
        {
            br := f.boundRect(false)
            boundRects.add(br)

            loop x: undo.newImg.width
            {
                var col1, col2: Color
                pt := Math.Point{cast(f32) x, 0}
                y1 := cast(s32) (br.bottom() - capture.backImagePos.y)
                y1 = undo.newImg.height - y1
                loop i: undo.newImg.height
                {
                    y1 += 1
                    pt.y = cast(f32) (capture.backImg.height - y1) + capture.backImagePos.y
                    if br.contains(pt) and f.isInside(pt)
                    {
                        pix32 := undo.newImg.pixels.buffer + x * undo.newImg.bpp8
                        pix32 += y1 * undo.newImg.width * undo.newImg.bpp8
                        col1.argb = dref cast(*u32) pix32
                        break
                    }
                }

                y2 := cast(s32) (br.y - capture.backImagePos.y)
                y2 = undo.newImg.height - y2
                loop i: undo.newImg.height
                {
                    y2 -= 1
                    pt.y = cast(f32) (capture.backImg.height - y2) + capture.backImagePos.y
                    if br.contains(pt) and f.isInside(pt)
                    {
                        pix32 := undo.newImg.pixels.buffer + x * undo.newImg.bpp8
                        pix32 += y2 * undo.newImg.width * undo.newImg.bpp8
                        col2.argb = dref cast(*u32) pix32
                        break
                    }
                }

                boundCols.add(@{y1, y2, col1, col2})
            }
        }

        params := @{capture, boundRects: boundRects.buffer, boundCols: boundCols.buffer, forms: &forms}
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
                    p1   := p.boundCols[x]
                    col1 := p1.col1
                    col2 := p1.col2
                    y1   := p1.y1
                    y2   := p1.y2

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