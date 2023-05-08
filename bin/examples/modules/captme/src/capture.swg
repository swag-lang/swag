using Serialization

const PreviewHeight = 256.0
const PreviewWidth  = PreviewHeight * (4/3.0)

enum MouseMode
{
    None
    Moving
    Creating
    Selecting
}

struct Capture
{
    const MinSize = 16
    const MaxSize = 4096

    // To save
    width, height:          s32
    backImg:                Image
    widthOrg, heightOrg:    s32
    backImgOrg:             Image

    backImagePos:   Math.Point
    preview:        Image
    file:           String
    forms:          Array'IForm
    nextStepIndex:  u32 = 1

    linkedImage:    String

    // Dyn
    #[NoSerialize]
    {
        changed:    bool
        mouseMode:  MouseMode = None
        editView:   *EditView
        gizmo:      GizmoRect
        undos:      ArrayPtr'Undo
        nextUndo:   u64
        hotForm:    u64 = Swag.U64.Max
        selection:  Array'u64
        paintRect:  Math.Rectangle

        backTexture: Texture
        snapX:       f32 = -1
        snapY:       f32 = -1

        serialUndo:         u32
        serialBackTexture:  u32
        movingStart:        Math.Point
        movingEnd:          Math.Point
        firstMouseMove:     bool
        movingCursorSet:    bool
        dirtyBackImg:       bool
        drawSelRect:        bool
    }
}

impl Capture
{
    func freeForms(forms: Array'IForm)
    {
        visit it: forms
        {
            type := @kindof(it)
            Memory.delete(@dataof(it), type)
        }
    }

    mtd opDrop()
    {
        freeForms(forms)
    }

    mtd setup(view: *EditView)
    {
        editView = view
        gizmo.sigChanged = closure|self|(g, first, hitMove) {
            undo := self.newUndo(.MoveCapture)
            undo.newPosX  = cast(s32) ((g.rect.x - g.movingRect.x) / self.editView.zoom)
            undo.newPosY  = cast(s32) ((g.rect.y - g.movingRect.y) / self.editView.zoom)
            undo.newSizeX = cast(s32) ((g.movingRect.width - g.rect.width) / self.editView.zoom)
            undo.newSizeY = cast(s32) ((g.movingRect.height - g.rect.height) / self.editView.zoom)
            self.pushUndo(undo)
            self.editView.invalidate()
        }
    }

    //////////////////////////////
    // IMAGE
    //////////////////////////////

    mtd computePreview()
    {
        zoom := Math.min(PreviewWidth / width, PreviewHeight / height)
        preview = toImage(zoom)
        preview.applyKernel(.GaussianBlur3x3A)
    }

    mtd toImage(zoom: f32 = 1, onlySel = false)->Image
    {
        var result: retval

        rect := Math.Rectangle{width: width * zoom, height: height * zoom}
        var painter: Pixel.Painter

        app := editView.getApp()
        render := app.getRenderer()

        painter.begin()
        painter.setColorMaskAlpha()
        painter.clear(Argb.Zero)
        painter.setColorMaskFull()
        paintBackImage(&painter, 0, 0, zoom)
        painter.setColorMaskColor()
        painter.setParams(.Antialiased)

        if onlySel
        {
            visit sel: selection
            {
                f := forms[sel]
                b := cast(*BaseForm) @dataof(f)
                if b.subKind == .PixSel
                    continue
                paintForm(app, &painter, rect, f, zoom, snapMode: true)
            }
        }
        else
        {
            visit f: forms
            {
                b := cast(*BaseForm) @dataof(f)
                if b.subKind == .PixSel
                    continue
                paintForm(app, &painter, rect, f, zoom, snapMode: true)
            }
        }

        painter.end()

        render.begin(editView.surface.rc)

        rt := render.getRenderTarget(cast(s32) rect.width, cast(s32) rect.height)
        render.bindRenderTarget(rt)
        render.setScissorRect(rect)
        render.clear()
        render.draw(&painter)
        render.unbindRenderTarget()
        render.releaseRenderTarget(rt)

        render.end()

        buffer := render.getTexturePixels(rt.colorBuffer, .BGRA8)
        result = Image.create(rt.colorBuffer.width, rt.colorBuffer.height, .BGRA8, buffer.toSlice())
        result.crop(0, 0, cast(s32) rect.width, cast(s32) rect.height)

        return result
    }

    //////////////////////////////
    // FILE
    //////////////////////////////

    mtd hasChanged()
    {
        changed = true
        editView.main.captureHasChanged()
    }

    mtd save() throw
    {
        if !changed
            return
        changed = false

        var buf:     ConcatBuffer
        var encoder: Encoder'Write.TagBin

        encoder.writeAll(&buf, dref self)
        File.writeAllBytes(file, buf)
    }

    func load(file: string)->*Capture throw
    {
        res := File.readAllBytes(file)

        var decoder: Decoder'Read.TagBin
        decoder.flags |= .IgnoreStructFieldError
        capture := decoder.readAll'Capture(res.toSlice())
        capture.file = file

        visit itf: capture.forms
            itf.setup(capture)

        return capture
    }

    //////////////////////////////
    // CLIPBOARD
    //////////////////////////////

    mtd copyAll()
    {
        Cursor.wait()
        Clipboard.addImage(toImage())
    }

    mtd copy()
    {
        var arr: Array'IForm

        imgDone := false
        visit s: selection
        {
            itf := forms[s]

            // Add real image
            if !imgDone and @kindof(itf) == FormImage
            {
                fimg := cast(*FormImage) @dataof(itf)
                if fimg.kind == .Image and fimg.image.isValid()
                {
                    imgDone = true
                    Clipboard.addImage(fimg.image)
                }
            }

            arr.add(itf)
        }

        var buf:    ConcatBuffer
        var encode: Encoder'Write.TagBin

        trycatch
        {
            encode.start(&buf)
            encode.writeValue(&arr, @typeof(arr))
            encode.end()
        }

        buf.makeLinear()
        Clipboard.addData(editView.main.clipFmt, buf.toSlice())
    }

    mtd pasteImageInside()
    {
        img := Clipboard.getImage()
        if !img.isValid()
            return

        undo := newUndo(.SetImageInside)
        undo.oldImg = img
        pushUndo(undo)
        selectionHasChanged()
    }

    mtd clearImageInside()
    {
        undo := newUndo(.SetImageInside)
        pushUndo(undo)
        selectionHasChanged()
    }

    mtd paste()
    {
        var decode: Decoder'Read.TagBin
        var res:    Array'IForm

        arr := Clipboard.getData(editView.main.clipFmt)
        if arr.count
        {
            trycatch
            {
                decode.start(arr.toSlice())
                decode.readValue(&res, Array'IForm)
                if !res.count return
                decode.end()
            }
        }
        else
        {
            img := Clipboard.getImage()
            if img.isValid()
            {
                formImg := Memory.new'FormImage()
                formImg.w = img.width
                formImg.h = img.height
                formImg.image = img
                res.add(cast(IForm) formImg)
            }
            else
                return
        }

        undo := newUndo(.CreateForm)
        selection.clear()

        var boundRect: Math.Rectangle

        visit &itf, idx: res
        {
            itf.setup(self)
            bound := itf.boundRect(false)
            if idx == 0
                boundRect = bound
            else
                boundRect.setUnion(bound)
        }

        undo.oldForms = res

        // Move around mouse
        pt := editView.getApp().mouse.getPosition()
        pt = editView.screenToSurface(pt)
        pt = editView.surfaceToLocal(pt)
        pt += editView.scrollPos
        pt.x -= paintRect.x
        pt.y -= paintRect.y
        zoom := paintRect.width / width
        pt.x /= zoom
        pt.y /= zoom

        visit itf: undo.oldForms
        {
            dx := -boundRect.x + pt.x - (boundRect.width * 0.5)
            dy := -boundRect.y + pt.y - (boundRect.height * 0.5)
            itf.moveForm(acast dx, acast dy)
        }

        countBefore := forms.count
        pushUndo(undo)

        // Select added forms
        selection.clear()
        loop i: undo.oldForms
            selection.add(countBefore + i)

        selectionHasChanged()
    }

    //////////////////////////////
    // PAINT
    //////////////////////////////

    func paintForm(app: *Application, painter: *Painter, rect: Math.Rectangle, f: IForm, zoom: f32, quickMode = false, snapMode = false)
    {
        render := &app.renderer

        boundRect := f.boundRect(false)
        if boundRect.width <= 0 or boundRect.height <= 0
            return

        base := cast(*BaseForm) @dataof(f)

        savePaintPos := base.paintPos

        base.paintPos.x      = rect.x + (boundRect.x * zoom)
        base.paintPos.y      = rect.y + (boundRect.y * zoom)
        base.paintPos.width  = boundRect.width * zoom
        base.paintPos.height = boundRect.height * zoom

        base.paintPos.width  += base.paintPos.x - Math.round(base.paintPos.x)
        base.paintPos.height += base.paintPos.y - Math.round(base.paintPos.y)
        base.paintPos.round()

        wt2 := cast(s32) base.paintPos.width
        ht2 := cast(s32) base.paintPos.height
        wt  := wt2
        ht  := ht2

        rt := render.getRenderTarget(wt2, ht2)
        painter.bindRenderTarget(rt, true)
        painter.setAntialiased()
        painter.clear(Argb.Zero)
        painter.setColorMaskColor()
        painter.setBlendingMode(.Alpha)
        f.paint(app, painter, zoom, quickMode)
        painter.unbindRenderTarget()

        const Margin = 8

        var rt1, rt2: *RenderTarget
        if base.paintShadow
        {
            wt += 2*Margin
            ht += 2*Margin

            rt1 = render.getRenderTarget(wt, ht)
            rt2 = render.getRenderTarget(wt, ht)

            // Draw the image with a margin around
            srcRect := Math.Rectangle{0, 0, base.paintPos.width, base.paintPos.height}
            painter.bindRenderTarget(rt1)
            painter.setShader(&render.shaderSimple)
            painter.setBlendingMode(.Copy)
            painter.setColorMaskFull()
            painter.clear(Argb.Zero)
            dstRect := Math.Rectangle{Margin, Margin, wt2, ht2}
            painter.drawTexture(dstRect, srcRect, rt.colorBuffer)
            painter.unbindRenderTarget()

            blurRadius := quickMode ? 4.0 : 8.0

            // Blur first pass
            painter.bindRenderTarget(rt2)
            painter.setShader(&render.shaderBlur)
            painter.setShaderParam(render.shaderBlur.axis, 0's32)
            painter.setShaderParam(render.shaderBlur.radius, blurRadius)
            painter.setBlendingMode(.Copy)
            painter.setColorMaskFull()
            painter.clear(Argb.Zero)
            dstRect = Math.Rectangle{0, 0, wt, ht}
            painter.drawTexture(dstRect, dstRect, rt1.colorBuffer)
            painter.setShader()
            painter.unbindRenderTarget()

            // Blur second pass
            painter.bindRenderTarget(rt1)
            painter.setShader(&render.shaderBlur)
            painter.setShaderParam(render.shaderBlur.axis, 1's32)
            painter.setShaderParam(render.shaderBlur.radius, blurRadius)
            painter.setBlendingMode(.Copy)
            painter.setColorMaskFull()
            dstRect = Math.Rectangle{0, 0, wt, ht}
            painter.drawTexture(dstRect, dstRect, rt2.colorBuffer, Color.fromArgb(150, Argb.Black))

            // Add the real image at the right place just after
            imgRect := dstRect
            imgRect.offset(cast(f32) Margin)
            imgRect.width = base.paintPos.width
            imgRect.height = base.paintPos.height
            imgRect.offset(-base.shadowOffset.x, -base.shadowOffset.y)
            painter.setShader(&render.shaderSimple)
            painter.setBlendingMode(.Alpha)
            painter.drawTexture(imgRect, srcRect, rt.colorBuffer)
            painter.unbindRenderTarget()
        }

        // Paint resulting image
        painter.pushClippingRect(rect)
        painter.setShader()
        painter.setAntialiased(false)

        var colForm: Color
        if quickMode and base.subKind == .Highlight
            colForm = Argb.White
        elif quickMode and base.subKind == .PixSel
            colForm = Argb.White
        else
            colForm = Color.fromArgb(cast(u8) Math.map(cast(f32) base.alpha, 0, 100, 0, 255), Argb.White)

        dstRect := Math.Rectangle{0, 0, wt, ht}
        if base.paintShadow
        {
            paintShadowRect := dstRect
            paintShadowRect.x = base.paintPos.x - Margin
            paintShadowRect.y = base.paintPos.y - Margin
            paintShadowRect.offset(base.shadowOffset.x, base.shadowOffset.y)
            painter.setColorMaskColor()
            painter.drawTexture(paintShadowRect, dstRect, rt1.colorBuffer, colForm)

            painter.setBlendingMode(.Max)
            painter.setColorMaskAlpha()
            painter.drawTexture(paintShadowRect, dstRect, rt1.colorBuffer, colForm)
        }
        else
        {
            paintRect := dstRect
            paintRect.x = base.paintPos.x
            paintRect.y = base.paintPos.y
            painter.setColorMaskColor()
            painter.setInterpolationMode(.Pixel)
            painter.drawTexture(paintRect, dstRect, rt.colorBuffer, colForm)

            painter.setBlendingMode(.Max)
            painter.setColorMaskAlpha()
            painter.drawTexture(paintRect, dstRect, rt.colorBuffer, colForm)
            painter.setInterpolationMode(.Linear)
        }

        painter.setColorMaskColor()
        painter.setBlendingMode(.Alpha)
        painter.popClippingRect()

        painter.releaseRenderTarget(rt)
        painter.releaseRenderTarget(rt1)
        painter.releaseRenderTarget(rt2)

        if snapMode
            base.paintPos = savePaintPos
    }

    mtd paintBackImage(painter: *Painter, x, y: f32, z: f32)
    {
        renderer := editView.getApp().getRenderer()

        // Background image
        if !backTexture.isValid() and backImg.width > 0
        {
            backTexture = renderer.addImage(backImg)
            serialBackTexture += 1
        }

        if backTexture.isValid()
        {
            // Background image has been changed
            if dirtyBackImg
            {
                renderer.removeTexture(&backTexture)
                backTexture = renderer.addImage(backImg)
                dirtyBackImg = false
                serialBackTexture += 1
            }

            var backRect: Math.Rectangle
            backRect.x = (backImagePos.x * z) + x
            backRect.y = (backImagePos.y * z) + y
            backRect.width = backImg.width * z
            backRect.height = backImg.height * z
            painter.drawTexture(backRect, backTexture)
        }
    }

    mtd paint(painter: *Painter, rect: Math.Rectangle)
    {
        painter.fillRect(rect, Argb.LightGray)
        painter.fillRect(rect, Brush.createHatch(.CheckBoardLarge, Argb.DarkGray))
        painter.setParams(.Antialiased)

        paintRect = rect
        zoom := rect.width / width

        // Background image
        painter.pushClippingRect(rect)
        painter.setAntialiased(false)
        paintBackImage(painter, rect.x, rect.y, zoom)
        painter.popClippingRect()

        // Shapes
        app := editView.getApp()
        visit f: forms
            paintForm(app, painter, rect, f, zoom)

        // Gizmos
        visit f, i: forms
        {
            flags := FormFlags.Zero
            if i == hotForm
                flags |= .Hot
            if selection.contains(i)
                flags |= .Selected
            f.paintGizmo(painter, zoom, flags)
        }

        // Selection rectangle
        if drawSelRect
        {
            with var r: Math.Rectangle
            {
                .x = movingStart.x
                .y = movingStart.y
                .width = movingEnd.x - movingStart.x
                .height = movingEnd.y - movingStart.y
            }

            painter.fillRect(r, Color.fromArgb(0x1f, Argb.Cyan))
            painter.drawRect(r, Argb.Cyan, 2)
        }

        // Snap
        if snapX != -1
            painter.drawLine(rect.x + snapX * zoom, rect.y, rect.x + snapX * zoom, rect.bottom(), Argb.Purple, 2)
        if snapY != -1
            painter.drawLine(rect.x, rect.y + snapY * zoom, rect.right(), rect.y + snapY * zoom, Argb.Purple, 2)
    }

    //////////////////////////////
    // SNAP
    //////////////////////////////

    mtd getSnapPoint(bound: Math.Rectangle, right, bottom: bool, nx, ny: s32, idxPt: u32, editForm: IForm = null)->Math.Point
    {
        var result: retval

        var form: IForm
        var type: typeinfo

        if editForm
            form = editForm
        else
            form = forms[selection[0]]

        type = @kindof(form)

        if selection.count == 1 and type == FormPolygon
        {
            poly := cast(*FormPolygon) @dataof(form)
            result.x = poly.points[idxPt].x + nx
            result.y = poly.points[idxPt].y + ny
        }
        elif (selection.count == 1 or editForm) and (type == FormLine or type == FormArrow)
        {
            line := cast(*FormLine) @dataof(form)
            if !right or !bottom
            {
                result.x = line.xa
                result.y = line.ya
            }
            else
            {
                result.x = line.xb
                result.y = line.yb
            }

            result.x += nx
            result.y += ny
        }
        else
        {
            result.x = right ? bound.right() : bound.x
            result.y = bottom ? bound.bottom() : bound.y
            result.x += nx
            result.y += ny
        }

        return result
    }

    mtd getSelectionBoundRect(editForm: IForm = null)->Math.Rectangle
    {
        if editForm
            return editForm.boundRect(true)

        bound := Math.Rectangle{Swag.F32.Max, Swag.F32.Max, 0, 0}
        visit s: selection
        {
            f := forms[s]
            r := f.boundRect(true)
            bound.x = Math.min(bound.x, r.x)
            bound.y = Math.min(bound.y, r.y)
            bound.setRight(Math.max(bound.right(), r.right()))
            bound.setBottom(Math.max(bound.bottom(), r.bottom()))
        }

        return bound
    }

    mtd snap(x, y: *s32, right, bottom: bool, idxPt: u32 = 0, editForm: IForm = null)
    {
        const SnapToler = 10

        if x
            snapX = -1
        if y
            snapY = -1

        ctrlPressed := Input.Keyboard.isKeyPressed(.Control) or Input.Keyboard.isKeyPressed(.RControl)
        if !editView.main.snap and !ctrlPressed
            return

        nx := x ? dref x : 0
        ny := y ? dref y : 0

        bound := getSelectionBoundRect(editForm)

        var arrX: Array's32
        var arrY: Array's32

        arrX.add(0)
        arrX.add(width)
        arrX.add(width / 2)
        arrY.add(0)
        arrY.add(height)
        arrY.add(height / 2)

        visit f, i: forms
        {
            autoSel := selection.contains(i) or @dataof(f) == @dataof(editForm)
            form    := cast(*BaseForm) @dataof(f)

            switch @kindof(f)
            {
            case FormLine, FormArrow:
                form2 := cast(*FormLine) form
                if !autoSel or (idxPt != 0 and Math.abs(form2.ya - form2.yb) > 25)
                    arrX.add(cast(s32) form2.xa)
                if !autoSel or (idxPt != 0 and Math.abs(form2.xa - form2.xb) > 25)
                    arrY.add(cast(s32) form2.ya)

                if !autoSel or idxPt != 1
                {
                    arrX.add(cast(s32) form2.xb)
                    arrY.add(cast(s32) form2.yb)
                }

            case FormPolygon:
                form2 := cast(*FormPolygon) form
                visit pt, idx: form2.points
                {
                    if !autoSel or idx != idxPt
                    {
                        arrX.add(cast(s32) pt.x)
                        arrY.add(cast(s32) pt.y)
                    }
                }

            case FormDraw:
                if autoSel
                    continue
                boundR := f.boundRect(false)
                arrX.add(cast(s32) boundR.x)
                arrX.add(cast(s32) boundR.right())
                arrY.add(cast(s32) boundR.y)
                arrY.add(cast(s32) boundR.bottom())

            default:
                if autoSel
                    continue
                arrX.add(cast(s32) form.x)
                arrX.add(cast(s32) (form.x + form.w))
                arrY.add(cast(s32) form.y)
                arrY.add(cast(s32) (form.y + form.h))
            }
        }

        bestX := Swag.F32.Max
        bestY := Swag.F32.Max
        bestCoordX := 0.0
        bestCoordY := 0.0

        if x
        {
            visit ax: arrX
            {
                p   := getSnapPoint(bound, right, bottom, nx, ny, idxPt, editForm)
                nbX := Math.abs(p.x - ax)
                if nbX < bestX
                {
                    bestX = nbX
                    bestCoordX = ax
                }
            }
        }

        if y
        {
            visit ay: arrY
            {
                p   := getSnapPoint(bound, right, bottom, nx, ny, idxPt, editForm)
                nbY := Math.abs(p.y - ay)
                if nbY < bestY
                {
                    bestY = nbY
                    bestCoordY = ay
                }
            }
        }

        if x and bestX < SnapToler
        {
            snapX = bestCoordX
            p := getSnapPoint(bound, right, bottom, 0, 0, idxPt, editForm)
            nx = cast(s32) (bestCoordX - p.x)
        }

        if y and bestY < SnapToler
        {
            snapY = bestCoordY
            p := getSnapPoint(bound, right, bottom, 0, 0, idxPt, editForm)
            ny = cast(s32) (bestCoordY - p.y)
        }

        if x dref x = nx
        if y dref y = ny
    }

    //////////////////////////////
    // CONTENT
    //////////////////////////////

    mtd popupMenu(view: *EditView, pos: Math.Point)
    {
        with menu := MenuCtrl.createPopup(view)
        {
            if selection.count
            {
                f := forms[selection.back()]
                t := @kindof(f)
                switch t
                {
                case FormText:
                    .addItem(CmdEditText)
                    .addSeparator()
                }
            }

            .addItem(CmdIdCopy)
            .addItem(CmdIdPaste)
            .addSeparator()
            .addItem(CmdIdPasteImageInside)
            .addItem(CmdIdClearImageInside)
            .addSeparator()
            .addItem(CmdIdDeleteSelection)
            .addSeparator()
        }

        with popup0 := MenuCtrl.createPopup(view)
        {
            .addItem(CmdIdAlignLeft)
            .addItem(CmdIdAlignCenter)
            .addItem(CmdIdAlignRight)
            .addSeparator()
            .addItem(CmdIdAlignTop)
            .addItem(CmdIdAlignMiddle)
            .addItem(CmdIdAlignBottom)
            menu.addPopup("Align", popup0)
        }

        with popup1 := MenuCtrl.createPopup(view)
        {
            .addItem(CmdIdBringToFront)
            .addItem(CmdIdSendToBack)
            .addItem(CmdIdBringForward)
            .addItem(CmdIdSendBackward)
            menu.addPopup("Arrange", popup1)
        }

        with menu
        {
            .addSeparator()
            .addItem(CmdIdCopyAsImage)
            .addItem(CmdIdSelectAll)
            .addItem(CmdIdDeSelectAll)
            .addSeparator()
            .addItem(CmdIdFlatten)
            .addItem(CmdIdFlattenAll)
            .addSeparator()
            .addItem(CmdIdSnap)
        }

        discard menu.doModal(pos)
        view.setFocus()
    }

    mtd mouse(view: *EditView, evt: *MouseEvent)->bool
    {
        app := view.getApp()

        localPos := view.surfaceToLocal(evt.surfacePos)
        localPos += view.scrollPos

        if mouseMode == .None
        {
            // Resize capture
            snapX, snapY = -1
            if gizmo.mouse(view, localPos, evt)
            {
                view.invalidate()
                evt.accepted = true
                if evt.kind != .MouseLeave
                    return true
            }

            // Specific moves only if one single selection
            if selection.count == 1
            {
                selForm := forms[selection[0]]
                if selForm.mouse(view, localPos, evt)
                {
                    evt.accepted = true
                    if evt.kind != .MouseLeave
                        return true
                }
            }

            // Popup
            if evt.kind == .MousePressed and evt.button == .Right
            {
                if movingCursorSet
                {
                    movingCursorSet = false
                    app.popCursor()
                }

                popupMenu(view, evt.surfacePos)
                return true
            }

            if evt.kind == .MouseLeave
            {
                hotForm = Swag.U64.Max
            }
            else
            {
                ht := hitTest(localPos)
                if ht != hotForm
                {
                    view.invalidate()
                    hotForm = ht
                }
            }

            if hotForm != Swag.U64.Max
            {
                if !movingCursorSet
                {
                    movingCursorSet = true
                    app.pushCursor(Cursor.from(.SizeAll))
                }
            }
            elif movingCursorSet
            {
                movingCursorSet = false
                app.popCursor()
            }
        }

        shiftPressed := Input.Keyboard.isKeyPressed(.Shift) or Input.Keyboard.isKeyPressed(.RShift)
        ctrlPressed  := Input.Keyboard.isKeyPressed(.Control) or Input.Keyboard.isKeyPressed(.RControl)

        // Start of mode
        ///////////////////////
        if mouseMode == .None and evt.button == .Left
        {
            if evt.kind == .MousePressed or evt.kind == .MouseDoubleClick
            {
                if (shiftPressed and ctrlPressed) or view.main.editMode == .Selection
                {
                    mouseMode = .Selecting
                    firstMouseMove = true
                    movingStart = localPos
                    view.captureMouse()
                    view.invalidate()
                    drawSelRect = true
                }
                else
                {
                    hadSelection := selection.count != 0
                    here := selection.contains(hotForm)
                    if !(evt.modifiers & .Control) and !here
                    {
                        if selection.count
                        {
                            selection.clear()
                            selectionHasChanged()
                        }
                    }

                    if hotForm != Swag.U64.Max
                    {
                        if !here
                        {
                            selection.add(hotForm)
                            selectionHasChanged()
                        }

                        mouseMode = .Moving
                        firstMouseMove = true
                        movingStart = localPos
                        view.captureMouse()
                        view.invalidate()
                    }
                    else if !hadSelection or editView.main.editMode != .Step
                    {
                        mouseMode = .Creating
                        firstMouseMove = true
                        movingStart = localPos
                        view.captureMouse()
                        view.invalidate()
                    }
                }
            }
        }

        // End of mode
        ///////////////////////
        if evt.kind == .MouseReleased and evt.button == .Left
        {
            snapX, snapY = -1

            if mouseMode == .Selecting
            {
                view.releaseMouse()
                view.invalidate()
                mouseMode = .None
                drawSelRect = false
                view.main.updateState()
            }

            if mouseMode == .Moving and hotForm == Swag.U64.Max
            {
                app.popCursor()
                movingCursorSet = false
            }

            if mouseMode == .Moving or firstMouseMove
            {
                view.releaseMouse()
                view.invalidate()
                mouseMode = .None
            }
        }

        // Mode
        ///////////////////////
        switch mouseMode
        {
        case Selecting:
            var r: Math.Rectangle

            if !ctrlPressed
                selection.clear()
            movingEnd = localPos

            r.set(movingStart, movingEnd)

            visit f, idx: forms
            {
                b := cast(*BaseForm) @dataof(f)
                if b.paintPos.intersectWith(r) and !selection.contains(idx)
                    selection.add(idx)
            }

            firstMouseMove = false
            view.invalidate()
            selectionHasChanged()
            return true

        case Moving:
            newPosX := cast(s32) ((localPos.x - movingStart.x) / view.zoom)
            newPosY := cast(s32) ((localPos.y - movingStart.y) / view.zoom)

            if firstMouseMove
            {
                if Math.abs(newPosX) <= 3 and Math.abs(newPosY) <= 3
                    return true
                firstMouseMove = false
            }
            else
                undo()

            u := newUndo(.OffsetSelection)

            // Snap by bottom/right
            nx := newPosX
            ny := newPosY
            snap(&nx, &ny, true, true)
            saveSnapX := snapX
            saveSnapY := snapY

            // Snap by top/left
            nx1 := newPosX
            ny1 := newPosY
            snap(&nx1, &ny1, false, false)

            if Math.abs(nx - newPosX) > Math.abs(nx1 - newPosX)
            {
                snapX = saveSnapX
                u.newPosX = nx
            }
            else
                u.newPosX = nx1
            if Math.abs(ny - newPosY) > Math.abs(ny1 - newPosY)
            {
                snapY = saveSnapY
                u.newPosY = ny
            }
            else
                u.newPosY = ny1

            pushUndo(u)
            view.invalidate()
            return true

        case Creating:
            newPosX := cast(s32) ((localPos.x - movingStart.x) / view.zoom)
            newPosY := cast(s32) ((localPos.y - movingStart.y) / view.zoom)

            if firstMouseMove
            {
                if Math.abs(newPosX) <= 3 and Math.abs(newPosY) <= 3 and editView.main.editMode != .Step
                    return true
                firstMouseMove = false

                var itf: IForm

                mode := editView.main.getEditMode()

                #[Swag.Complete]
                switch mode
                {
                case Shape:
                    form := Memory.new'FormShape()
                    dref form = editView.main.mdlFormShape
                    itf = cast(IForm) form
                case Image:
                    form := Memory.new'FormImage()
                    dref form = editView.main.mdlFormImage
                    form.image.release()
                    itf = cast(IForm) form
                case Step:
                    form := Memory.new'FormStep()
                    dref form = editView.main.mdlFormStep
                    itf = cast(IForm) form
                case Arrow:
                    form := Memory.new'FormArrow()
                    dref form = editView.main.mdlFormArrow
                    itf = cast(IForm) form
                case Line:
                    form := Memory.new'FormLine()
                    dref form = editView.main.mdlFormLine
                    itf = cast(IForm) form
                case Polygon:
                    form := Memory.new'FormPolygon()
                    dref form = editView.main.mdlFormPolygon
                    form.points.clear()
                    itf = cast(IForm) form
                case Text:
                    form := Memory.new'FormText()
                    dref form = editView.main.mdlFormText
                    itf = cast(IForm) form
                case Draw:
                    form := Memory.new'FormDraw()
                    dref form = editView.main.mdlFormDraw
                    form.points.clear()
                    itf = cast(IForm) form
                case Favorites, Highlight, Selection, PixSel:
                    break
                }

                itf.setup(self)

                undo := newUndo(.CreateForm)
                undo.oldForms.add(itf)
                pushUndo(undo)
            }

            f := forms.back()
            if !f.edit(evt, paintRect, movingStart, localPos, view.zoom)
            {
                view.releaseMouse()
                view.invalidate()
                mouseMode = .None
            }

            hasChanged()
            return true
        }

        return false
    }

    mtd hitTest(localPos: Math.Point)->u64
    {
        if !forms.count
            return Swag.U64.Max
        loop i: forms.count-1..0
        {
            f := forms[i]
            b := cast(*BaseForm) @dataof(f)
            if !b.paintPos.contains(localPos)
                continue
            if !f.hitTest(localPos, editView.zoom)
                continue
            return i
        }

        return Swag.U64.Max
    }

    //////////////////////////////
    // SELECTION
    //////////////////////////////

    mtd getSelectedBaseForm()->*BaseForm
    {
        if selection.count == 0
            return null
        return cast(*BaseForm) @dataof(forms[selection.back()])
    }

    mtd getSelectedFormType()->typeinfo
    {
        if selection.count == 0
            return null
        i := forms[selection.back()]
        return @kindof(i)
    }

    mtd updateEditModeFromSelection()
    {
        if !selection.count
            return

        main := editView.main
        if main.editMode == .Selection
            return

        f := forms[selection.back()]
        t := @kindof(f)
        form := cast(*BaseForm) @dataof(f)

        if main.editMode == .Favorites
        {
            #[Swag.Complete]
            switch t
            {
            case FormShape:
                main.mdlFavorite = main.mdlFormShape
            case FormImage:
                main.mdlFavorite = main.mdlFormImage
            case FormStep:
                main.mdlFavorite = main.mdlFormStep
            case FormArrow:
                main.mdlFavorite = main.mdlFormArrow
            case FormLine:
                main.mdlFavorite = main.mdlFormLine
            case FormPolygon:
                main.mdlFavorite = main.mdlFormPolygon
            case FormText:
                main.mdlFavorite = main.mdlFormText
            case FormDraw:
                main.mdlFavorite = main.mdlFormDraw
            }
        }
        elif form.subKind == .Highlight
        {
            main.editMode = .Highlight
        }
        elif form.subKind == .PixSel
        {
            main.editMode = .PixSel
        }
        else
        {
            #[Swag.Complete]
            switch t
            {
            case FormShape:
                main.editMode = .Shape
            case FormImage:
                main.editMode = .Image
            case FormStep:
                main.editMode = .Step
            case FormArrow:
                main.editMode = .Arrow
            case FormLine:
                main.editMode = .Line
            case FormPolygon:
                main.editMode = .Polygon
            case FormText:
                main.editMode = .Text
            case FormDraw:
                main.editMode = .Draw
            }
        }
    }

    mtd updateModelFromSelection()
    {
        if !selection.count
            return

        f := forms[selection.back()]
        t := @kindof(f)
        form := cast(*BaseForm) @dataof(f)
        if form.subKind == .Highlight
            editView.main.mdlHighlight = f
        elif form.subKind == .PixSel
            editView.main.mdlPixSel = f

        #[Swag.Complete]
        switch t
        {
        case FormShape:
            b := cast(*FormShape) @dataof(f)
            editView.main.mdlFormShape = dref b

        case FormImage:
            b := cast(*FormImage) @dataof(f)
            editView.main.mdlFormImage = dref b

        case FormStep:
            b := cast(*FormStep) @dataof(f)
            editView.main.mdlFormStep = dref b
            editView.main.mdlFormStep.ws = b.w
            editView.main.mdlFormStep.hs = b.h

        case FormArrow:
            b := cast(*FormArrow) @dataof(f)
            editView.main.mdlFormArrow = dref b

        case FormLine:
            b := cast(*FormLine) @dataof(f)
            editView.main.mdlFormLine = dref b

        case FormPolygon:
            b := cast(*FormPolygon) @dataof(f)
            editView.main.mdlFormPolygon = dref b

        case FormText:
            b := cast(*FormText) @dataof(f)
            editView.main.mdlFormText = dref b

        case FormDraw:
            b := cast(*FormDraw) @dataof(f)
            editView.main.mdlFormDraw = dref b
        }
    }

    mtd selectionHasChanged()
    {
        updateEditModeFromSelection()
        updateModelFromSelection()
        editView.main.updateState()
    }

    //////////////////////////////
    // UNDO
    //////////////////////////////

    mtd newUndo(kind: UndoKind, serial: u32 = 0)->*Undo
    {
        ptr := undos.newPtr()
        ptr.kind = kind
        if serial
            ptr.serial = serial
        else
        {
            serialUndo += 1
            ptr.serial = serialUndo
        }

        return ptr
    }

    mtd canUndo()->bool
    {
        return nextUndo != 0
    }

    mtd canRedo()->bool
    {
        return nextUndo != undos.count
    }

    mtd undo()
    {
        if !canUndo()
            return

        serial := undos[nextUndo-1].serial
        while nextUndo and undos[nextUndo-1].serial == serial
        {
            nextUndo -= 1
            undos[nextUndo].undo()
        }

        editView.invalidate()
    }

    mtd redo()
    {
        if !canRedo()
            return

        serial := undos[nextUndo].serial
        while nextUndo != undos.count and undos[nextUndo].serial == serial
        {
            undos[nextUndo].do()
            nextUndo += 1
        }

        editView.invalidate()
    }

    mtd pushUndo(u: *Undo)
    {
        u.capture = self

        // Remove no more relevant undos
        countUndo := nextUndo
        while nextUndo != undos.count
        {
            Memory.delete(undos[nextUndo])
            nextUndo += 1
        }

        // Add that one
        undos.resize(countUndo)
        undos.add(u)
        nextUndo = undos.count

        // Do it
        u.init()
        u.do()
    }
}