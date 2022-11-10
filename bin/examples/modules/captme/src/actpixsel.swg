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
                    pix[0], pix[1], pix[2], pix[3] = 0
                    break
                }
            }
        }

        capture.pushUndo(undo)
    }
}