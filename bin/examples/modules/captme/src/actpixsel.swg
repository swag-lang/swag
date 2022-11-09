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

        loop x: capture.backImg.width
        {
            loop y: capture.backImg.height
            {
                visit f, idx: forms
                {
                    pt := Math.Point{cast(f32) x, cast(f32) y}
                    pt.x += capture.backImagePos.x
                    pt.y += capture.backImagePos.y
                    if !boundRects[idx].contains(pt)
                        continue
                    if f.isInside(pt)
                    {
                        ptr := undo.newImg.pixels.buffer
                        ptr += x * undo.newImg.bpp8
                        ptr += (undo.newImg.height - y) * undo.newImg.bpp8 * undo.newImg.width
                        dref cast(*u32) ptr = 0
                    }
                }
            }
        }

        capture.pushUndo(undo)
    }
}