using Serialization

const PreviewHeight = 256.0
const PreviewWidth  = PreviewHeight * (4 / 3.0)

enum MouseMode
{
    None
    Moving
    Creating
    Selecting
}

struct Capture
{
    const MinSize            = 16
    const MaxSize            = 4096

    width, height:           s32
    backImg:                 Image
    widthOrg, heightOrg:     s32
    backImgOrg:              Image

    backImagePos:            Math.Point
    preview:                 Image
    file:                    String
    forms:                   Array'IForm
    nextStepIndex:           u32 = 1

    linkedImage:             String

    // Dyn
    #[NoSerialize]
    {
        changed:               bool
        mouseMode:             MouseMode = None
        editView:              *EditView
        gizmo:                 GizmoRect
        undos:                 ArrayPtr'Undo
        nextUndo:              u64
        hotForm:               u64 = Swag.U64.Max
        selection:             Array'u64
        paintRect:             Math.Rectangle

        backTexture:           Texture
        snapX:                 f32 = -1
        snapY:                 f32 = -1

        serialUndo:            u32
        serialBackTexture:     u32
        movingStart:           Math.Point
        movingEnd:             Math.Point
        firstMouseMove:        bool
        movingCursorSet:       bool
        dirtyBackImg:          bool
        drawSelRect:           bool
    }
}

impl Capture
{
    func freeForms(forms: Array'IForm)
    {
        foreach it in forms
        {
            let type = @kindof(it)
            Memory.delete(@dataof(it), type)
        }
    }

    mtd opDrop()
    {
        freeForms(forms)
    }

    mtd setup(view: *EditView)
    {
        editView         = view
        gizmo.sigChanged = func|self|(g, first, hitMove)
        {
            var undo = self.newUndo(.MoveCapture)
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
        let zoom = Math.min(PreviewWidth / width, PreviewHeight / height)
        preview = toImage(zoom)
        preview.applyKernel(.GaussianBlur3x3A)
    }

    mtd toImage(zoom: f32 = 1, onlySel = false)->Image
    {
        var result: retval

        var rect     = Math.Rectangle{width: width * zoom, height: height * zoom}
        var painter: Pixel.Painter

        let app    = editView.getApp()
        let render = app.getRenderer()

        painter.begin()
        painter.setColorMaskFull()
        painter.clear(Argb.Zero)
        paintBackImage(&painter, 0, 0, zoom)
        painter.setColorMaskColor()
        painter.setParams(.Antialiased)

        if onlySel
        {
            foreach sel in selection
            {
                let f = forms[sel]
                let b = cast(*BaseForm) @dataof(f)
                if b.subKind == .PixSel:
                    continue
                paintForm(app, &painter, rect, f, zoom, snapMode: true)
            }
        }
        else
        {
            foreach f in forms
            {
                let b = cast(*BaseForm) @dataof(f)
                if b.subKind == .PixSel:
                    continue
                paintForm(app, &painter, rect, f, zoom, snapMode: true)
            }
        }

        painter.end()

        render.begin(editView.surface.rc)

        let rt = render.getRenderTarget(cast(s32) rect.width, cast(s32) rect.height)
        render.bindRenderTarget(rt)
        render.setScissorRect(rect)
        render.clear()
        render.draw(&painter)
        render.unbindRenderTarget()
        render.releaseRenderTarget(rt)

        render.end()

        var buffer = render.getTexturePixels(rt.colorBuffer, .BGRA8)
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
        if !changed:
            return
        changed = false

        var buf:     ConcatBuffer
        var encoder: Encoder'Write.TagBin

        encoder.writeAll(&buf, dref self)
        File.writeAllBytes(file, buf)
    }

    func load(file: string)->*Capture throw
    {
        var res = File.readAllBytes(file)

        var decoder: Decoder'Read.TagBin
        decoder.flags |= .IgnoreStructFieldError
        let capture = decoder.readAll'Capture(res.toSlice())
        capture.file = file

        foreach itf in capture.forms:
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

        var imgDone = false
        foreach s in selection
        {
            let itf = forms[s]

            // Add real image
            if !imgDone and @kindof(itf) == FormImage
            {
                let fimg = cast(*FormImage) @dataof(itf)
                if fimg.kind == .Image and fimg.image.isValid()
                {
                    imgDone = true
                    Clipboard.addImage(fimg.image)
                }
            }

            arr.add(itf)
        }

        Clipboard.addTypedValue(&arr, #typeof(arr))
    }

    mtd pasteImageInside()
    {
        let img = Clipboard.getImage()
        if !img.isValid():
            return

        let undo = newUndo(.SetImageInside)
        undo.oldImg = img
        pushUndo(undo)
        selectionHasChanged()
    }

    mtd clearImageInside()
    {
        let undo = newUndo(.SetImageInside)
        pushUndo(undo)
        selectionHasChanged()
    }

    mtd paste()
    {
        var res: Array'IForm
        if !Clipboard.getTypedValue(&res, #typeof(res))
        {
            var img = Clipboard.getImage()
            if img.isValid()
            {
                let formImg = Memory.new'FormImage()
                formImg.w             = img.width
                formImg.h             = img.height
                formImg.image         = img
                formImg.borderColor.a = 0
                formImg.backColor.a   = 0
                res.add(cast(IForm) formImg)
            }
            else:
                return
        }

        let undo = newUndo(.CreateForm)
        selection.clear()

        var boundRect: Math.Rectangle

        foreach &itf, idx in res
        {
            itf.setup(self)
            var bound = itf.boundRect()
            if idx == 0:
                boundRect = bound
            else:
                boundRect.setUnion(bound)
        }

        undo.oldForms = res

        // Move around mouse
        var pt = editView.getApp().mouse.getPosition()
        pt = editView.screenToSurface(pt)
        pt = editView.surfaceToLocal(pt)
        pt += editView.scrollPos
        pt.x -= paintRect.x
        pt.y -= paintRect.y
        let zoom = paintRect.width / width
        pt.x /= zoom
        pt.y /= zoom

        foreach itf in undo.oldForms
        {
            let dx = -boundRect.x + pt.x - (boundRect.width * 0.5)
            let dy = -boundRect.y + pt.y - (boundRect.height * 0.5)
            itf.moveForm(cast() dx, cast() dy)
        }

        let countBefore = forms.count
        pushUndo(undo)

        // Select added forms
        selection.clear()
        for i in undo.oldForms:
            selection.add(countBefore + i)

        selectionHasChanged()
    }

    //////////////////////////////
    // PAINT
    //////////////////////////////

    func paintForm(app: nn *Application, painter: nn *Painter, rect: Math.Rectangle, f: nn IForm, zoom: f32, quickMode = false, snapMode = false)
    {
        let render = &app.renderer

        var boundRect = f.boundRect()
        if boundRect.width <= 0 or boundRect.height <= 0:
            return

        let base = cast(*BaseForm) @dataof(f)

        var savePaintPos = base.paintPos

        base.paintPos.x      = rect.x + (boundRect.x * zoom)
        base.paintPos.y      = rect.y + (boundRect.y * zoom)
        base.paintPos.width  = boundRect.width * zoom
        base.paintPos.height = boundRect.height * zoom

        base.paintPos.width += base.paintPos.x - Math.round(base.paintPos.x)
        base.paintPos.height += base.paintPos.y - Math.round(base.paintPos.y)
        base.paintPos.round()

        let wt2 = cast(s32) base.paintPos.width
        let ht2 = cast(s32) base.paintPos.height
        var wt  = wt2
        var ht  = ht2

        let rt = render.getRenderTarget(wt2, ht2)
        with painter
        {
            .bindRenderTarget(rt, true)
            .setAntialiased()
            .setColorMaskFull()
            .clear(Argb.Zero)
            .setColorMaskColor()
            .setBlendingMode(.Alpha)
            f.paint(app, painter, zoom, quickMode)
            .unbindRenderTarget()
        }

        const Margin = 8

        var rt1, rt2: *RenderTarget
        if base.paintShadow
        {
            wt += 2 * Margin
            ht += 2 * Margin

            rt1 = render.getRenderTarget(wt, ht)
            rt2 = render.getRenderTarget(wt, ht)

            // Draw the image with a margin around
            var srcRect = Math.Rectangle{0, 0, base.paintPos.width, base.paintPos.height}
            var dstRect = Math.Rectangle{Margin, Margin, wt2, ht2}
            with painter
            {
                .bindRenderTarget(rt1)
                .setShader(&render.shaderSimple)
                .setBlendingMode(.Copy)
                .setColorMaskFull()
                .clear(Argb.Zero)
                .drawTexture(dstRect, srcRect, rt.colorBuffer)
                .unbindRenderTarget()
            }

            let blurRadius = quickMode ? 4.0 : 8.0

            // Blur first pass
            with painter
            {
                .bindRenderTarget(rt2)
                .setShader(&render.shaderBlur)
                .setShaderParam(render.shaderBlur.axis, 0's32)
                .setShaderParam(render.shaderBlur.radius, blurRadius)
                .setBlendingMode(.Copy)
                .setColorMaskFull()
                .clear(Argb.Zero)
                .drawTexture(Math.Rectangle{0, 0, wt, ht}, Math.Rectangle{0, 0, wt, ht}, rt1.colorBuffer)
                .setShader()
                .unbindRenderTarget()
            }

            // Blur second pass
            with painter
            {
                .bindRenderTarget(rt1)
                .setShader(&render.shaderBlur)
                .setShaderParam(render.shaderBlur.axis, 1's32)
                .setShaderParam(render.shaderBlur.radius, blurRadius)
                .setBlendingMode(.Copy)
                .setColorMaskFull()
                .drawTexture(Math.Rectangle{0, 0, wt, ht}, Math.Rectangle{0, 0, wt, ht}, rt2.colorBuffer, Color.fromArgb(150, Argb.Black))
            }

            // Add the real image at the right place just after
            with var imgRect = Math.Rectangle{0, 0, wt, ht}
            {
                .offset(cast(f32) Margin)
                .width  = base.paintPos.width
                .height = base.paintPos.height
                .offset(-base.shadowOffset.x, -base.shadowOffset.y)
            }

            with painter
            {
                .setShader(&render.shaderSimple)
                .setBlendingMode(.Alpha)
                .drawTexture(imgRect, srcRect, rt.colorBuffer)
                .unbindRenderTarget()
            }
        }

        // Paint resulting image
        with painter
        {
            .pushClippingRect(rect)
            .setShader()
            .setAntialiased(false)
        }

        var colForm: Color
        if quickMode and base.subKind == .Highlight:
            colForm = Argb.White
        elif quickMode and base.subKind == .PixSel:
            colForm = Argb.White
        else:
            colForm = Color.fromArgb(cast(u8) Math.map(cast(f32) base.alpha, 0, 100, 0, 255), Argb.White)

        var dstRect = Math.Rectangle{0, 0, wt, ht}
        if base.paintShadow
        {
            with var paintShadowRect = dstRect
            {
                .x = base.paintPos.x - Margin
                .y = base.paintPos.y - Margin
                .offset(base.shadowOffset.x, base.shadowOffset.y)
            }

            with painter
            {
                .setColorMaskColor()
                .drawTexture(paintShadowRect, dstRect, rt1.colorBuffer, colForm)
                .setBlendingMode(.Max)
                .setColorMaskAlpha()
                .drawTexture(paintShadowRect, dstRect, rt1.colorBuffer, colForm)
            }
        }
        else
        {
            with var paintRect = dstRect
            {
                .x = base.paintPos.x
                .y = base.paintPos.y
            }

            with painter
            {
                .setColorMaskColor()
                .setInterpolationMode(.Pixel)
                .drawTexture(paintRect, dstRect, rt.colorBuffer, colForm)
                .setBlendingMode(.Max)
                .setColorMaskAlpha()
                .drawTexture(paintRect, dstRect, rt.colorBuffer, colForm)
                .setInterpolationMode(.Linear)
            }
        }

        with painter
        {
            .setColorMaskColor()
            .setBlendingMode(.Alpha)
            .popClippingRect()
            .releaseRenderTarget(rt)
            .releaseRenderTarget(rt1)
            .releaseRenderTarget(rt2)
        }

        if snapMode:
            base.paintPos = savePaintPos
    }

    mtd paintBackImage(painter: nn *Painter, x, y: f32, z: f32)
    {
        let renderer = editView.getApp().getRenderer()

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
                backTexture  = renderer.addImage(backImg)
                dirtyBackImg = false
                serialBackTexture += 1
            }

            with var backRect: Math.Rectangle
            {
                .x      = (backImagePos.x * z) + x
                .y      = (backImagePos.y * z) + y
                .width  = backImg.width * z
                .height = backImg.height * z
            }

            painter.drawTexture(backRect, backTexture)
        }
    }

    mtd paint(painter: nn *Painter, rect: Math.Rectangle)
    {
        painter.fillRect(rect, Argb.LightGray)
        painter.fillRect(rect, Brush.createHatch(.CheckBoardLarge, Argb.DarkGray))
        painter.setParams(.Antialiased)

        paintRect = rect
        let zoom = rect.width / width

        // Background image
        painter.pushClippingRect(rect)
        painter.setAntialiased(false)
        paintBackImage(painter, rect.x, rect.y, zoom)
        painter.popClippingRect()

        // Shapes
        let app = editView.getApp()
        foreach f in forms:
            paintForm(app, painter, rect, f, zoom)

        // Gizmos
        foreach f, i in forms
        {
            var flags = FormFlags.Zero
            if i == hotForm:
                flags |= .Hot
            if selection.contains(i):
                flags |= .Selected
            f.paintGizmo(painter, zoom, flags)
        }

        // Selection rectangle
        if drawSelRect
        {
            with var r: Math.Rectangle
            {
                .x      = movingStart.x
                .y      = movingStart.y
                .width  = movingEnd.x - movingStart.x
                .height = movingEnd.y - movingStart.y
            }

            painter.fillRect(r, Color.fromArgb(0x1f, Argb.Cyan))
            painter.drawRect(r, Argb.Cyan, 2)
        }

        // Snap
        if snapX != -1:
            painter.drawLine(rect.x + snapX * zoom, rect.y, rect.x + snapX * zoom, rect.bottom(), Argb.Purple, 2)
        if snapY != -1:
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

        if editForm:
            form = editForm
        else:
            form = forms[selection[0]]

        type = @kindof(form)

        if selection.count == 1 and type == FormPolygon
        {
            let poly = cast(*FormPolygon) @dataof(form)
            result.x = poly.points[idxPt].x + nx
            result.y = poly.points[idxPt].y + ny
        }
        elif (selection.count == 1 or editForm) and (type == FormLine or type == FormArrow)
        {
            let line = cast(*FormLine) @dataof(form)
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
        if editForm:
            return editForm.boundRect()

        var bound = Math.Rectangle{Swag.F32.Max, Swag.F32.Max, 0, 0}
        foreach s in selection
        {
            let f = forms[s]
            let r = f.boundRect()
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

        if x:
            snapX = -1
        if y:
            snapY = -1

        let ctrlPressed = Input.Keyboard.isKeyPressed(.Control) or Input.Keyboard.isKeyPressed(.RControl)
        if !editView.main.snap and !ctrlPressed:
            return

        var nx = x ? dref x : 0
        var ny = y ? dref y : 0

        var bound = getSelectionBoundRect(editForm)

        var arrX: Array's32
        var arrY: Array's32

        arrX.add(0)
        arrX.add(width)
        arrX.add(width / 2)
        arrY.add(0)
        arrY.add(height)
        arrY.add(height / 2)

        foreach f, i in forms
        {
            let autoSel = selection.contains(i) or @dataof(f) == @dataof(editForm)
            let form    = cast(*BaseForm) @dataof(f)

            switch f
            {
            case FormLine, FormArrow:
                let form2 = cast(*FormLine) form
                if !autoSel or (idxPt != 0 and Math.abs(form2.ya - form2.yb) > 25):
                    arrX.add(cast(s32) form2.xa)
                if !autoSel or (idxPt != 0 and Math.abs(form2.xa - form2.xb) > 25):
                    arrY.add(cast(s32) form2.ya)

                if !autoSel or idxPt != 1
                {
                    arrX.add(cast(s32) form2.xb)
                    arrY.add(cast(s32) form2.yb)
                }

            case FormPolygon:
                let form2 = cast(*FormPolygon) form
                foreach pt, idx in form2.points
                {
                    if !autoSel or idx != idxPt
                    {
                        arrX.add(cast(s32) pt.x)
                        arrY.add(cast(s32) pt.y)
                    }
                }

            case FormDraw:
                if autoSel:
                    continue
                let boundR = f.boundRect()
                arrX.add(cast(s32) boundR.x)
                arrX.add(cast(s32) boundR.right())
                arrY.add(cast(s32) boundR.y)
                arrY.add(cast(s32) boundR.bottom())

            default:
                if autoSel:
                    continue
                arrX.add(cast(s32) form.x)
                arrX.add(cast(s32) (form.x + form.w))
                arrY.add(cast(s32) form.y)
                arrY.add(cast(s32) (form.y + form.h))
            }
        }

        var bestX      = Swag.F32.Max
        var bestY      = Swag.F32.Max
        var bestCoordX = 0.0
        var bestCoordY = 0.0

        if x
        {
            foreach ax in arrX
            {
                let p   = getSnapPoint(bound, right, bottom, nx, ny, idxPt, editForm)
                let nbX = Math.abs(p.x - ax)
                if nbX < bestX
                {
                    bestX      = nbX
                    bestCoordX = ax
                }
            }
        }

        if y
        {
            foreach ay in arrY
            {
                let p   = getSnapPoint(bound, right, bottom, nx, ny, idxPt, editForm)
                let nbY = Math.abs(p.y - ay)
                if nbY < bestY
                {
                    bestY      = nbY
                    bestCoordY = ay
                }
            }
        }

        if x and bestX < SnapToler
        {
            snapX = bestCoordX
            let p = getSnapPoint(bound, right, bottom, 0, 0, idxPt, editForm)
            nx = cast(s32) (bestCoordX - p.x)
        }

        if y and bestY < SnapToler
        {
            snapY = bestCoordY
            let p = getSnapPoint(bound, right, bottom, 0, 0, idxPt, editForm)
            ny = cast(s32) (bestCoordY - p.y)
        }

        if x:
            dref x = nx
        if y:
            dref y = ny
    }

    //////////////////////////////
    // CONTENT
    //////////////////////////////

    mtd popupMenu(view: nn *EditView, pos: Math.Point)
    {
        with let menu = MenuCtrl.createPopup(view)
        {
            if selection.count
            {
                let f = forms[selection.back()]
                let t = @kindof(f)
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

        with let popup0 = MenuCtrl.createPopup(view)
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

        with let popup1 = MenuCtrl.createPopup(view)
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

    mtd mouse(view: nn *EditView, evt: nn *MouseEvent)->bool
    {
        let app = view.getApp()

        var localPos = view.surfaceToLocal(evt.surfacePos)
        localPos += view.scrollPos

        if mouseMode == .None
        {
            // Resize capture
            snapX, snapY = -1
            if gizmo.mouse(view, localPos, evt)
            {
                view.invalidate()
                evt.accepted = true
                if evt.kind != .MouseLeave:
                    return true
            }

            // Specific moves only if one single selection
            if selection.count == 1
            {
                let selForm = forms[selection[0]]
                if selForm.mouse(view, localPos, evt)
                {
                    evt.accepted = true
                    if evt.kind != .MouseLeave:
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
                let ht = hitTest(localPos)
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

        let shiftPressed = Input.Keyboard.isKeyPressed(.Shift) or Input.Keyboard.isKeyPressed(.RShift)
        let ctrlPressed  = Input.Keyboard.isKeyPressed(.Control) or Input.Keyboard.isKeyPressed(.RControl)

        // Start of mode
        ///////////////////////
        if mouseMode == .None and evt.button == .Left
        {
            if evt.kind == .MousePressed or evt.kind == .MouseDoubleClick
            {
                if (shiftPressed and ctrlPressed) or view.main.editMode == .Selection
                {
                    mouseMode      = .Selecting
                    firstMouseMove = true
                    movingStart    = localPos
                    view.captureMouse()
                    view.invalidate()
                    drawSelRect = true
                }
                else
                {
                    let hadSelection = selection.count != 0
                    let here         = selection.contains(hotForm)
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

                        mouseMode      = .Moving
                        firstMouseMove = true
                        movingStart    = localPos
                        view.captureMouse()
                        view.invalidate()
                    }
                    elif !hadSelection or editView.main.editMode != .Step
                    {
                        mouseMode      = .Creating
                        firstMouseMove = true
                        movingStart    = localPos
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
                mouseMode   = .None
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

            if !ctrlPressed:
                selection.clear()
            movingEnd = localPos

            r.set(movingStart, movingEnd)

            foreach f, idx in forms
            {
                let b = cast(*BaseForm) @dataof(f)
                if b.paintPos.intersectWith(r) and !selection.contains(idx):
                    selection.add(idx)
            }

            firstMouseMove = false
            view.invalidate()
            selectionHasChanged()
            return true

        case Moving:
            let newPosX = cast(s32) ((localPos.x - movingStart.x) / view.zoom)
            let newPosY = cast(s32) ((localPos.y - movingStart.y) / view.zoom)

            if firstMouseMove
            {
                if Math.abs(newPosX) <= 3 and Math.abs(newPosY) <= 3:
                    return true
                firstMouseMove = false
            }
            else:
                undo()

            let u = newUndo(.OffsetSelection)

            // Snap by bottom/right
            var nx = newPosX
            var ny = newPosY
            snap(&nx, &ny, true, true)
            let saveSnapX = snapX
            let saveSnapY = snapY

            // Snap by top/left
            var nx1 = newPosX
            var ny1 = newPosY
            snap(&nx1, &ny1, false, false)

            if Math.abs(nx - newPosX) > Math.abs(nx1 - newPosX)
            {
                snapX     = saveSnapX
                u.newPosX = nx
            }
            else:
                u.newPosX = nx1
            if Math.abs(ny - newPosY) > Math.abs(ny1 - newPosY)
            {
                snapY     = saveSnapY
                u.newPosY = ny
            }
            else:
                u.newPosY = ny1

            pushUndo(u)
            view.invalidate()
            return true

        case Creating:
            let newPosX = cast(s32) ((localPos.x - movingStart.x) / view.zoom)
            let newPosY = cast(s32) ((localPos.y - movingStart.y) / view.zoom)

            if firstMouseMove
            {
                if Math.abs(newPosX) <= 3 and Math.abs(newPosY) <= 3 and editView.main.editMode != .Step:
                    return true
                firstMouseMove = false

                var itf: IForm

                let mode = editView.main.getEditMode()

                #[Swag.Complete]
                switch mode
                {
                case Shape:
                    var form = Memory.new'FormShape()
                    dref form = editView.main.mdlFormShape
                    itf       = cast(IForm) form
                case Image:
                    var form = Memory.new'FormImage()
                    dref form = editView.main.mdlFormImage
                    form.image.release()
                    itf = cast(IForm) form
                case Step:
                    var form = Memory.new'FormStep()
                    dref form = editView.main.mdlFormStep
                    itf       = cast(IForm) form
                case Arrow:
                    var form = Memory.new'FormArrow()
                    dref form = editView.main.mdlFormArrow
                    itf       = cast(IForm) form
                case Line:
                    var form = Memory.new'FormLine()
                    dref form = editView.main.mdlFormLine
                    itf       = cast(IForm) form
                case Polygon:
                    var form = Memory.new'FormPolygon()
                    dref form = editView.main.mdlFormPolygon
                    form.points.clear()
                    itf = cast(IForm) form
                case Text:
                    var form = Memory.new'FormText()
                    dref form = editView.main.mdlFormText
                    itf       = cast(IForm) form
                case Draw:
                    var form = Memory.new'FormDraw()
                    dref form = editView.main.mdlFormDraw
                    form.points.clear()
                    itf = cast(IForm) form
                case Favorites, Highlight, Selection, PixSel:
                    break
                }

                itf.setup(self)

                let undo = newUndo(.CreateForm)
                undo.oldForms.add(itf)
                pushUndo(undo)
            }

            let f = forms.back()
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
        if !forms.count:
            return Swag.U64.Max
        for #back i in forms
        {
            let f = forms[i]
            let b = cast(*BaseForm) @dataof(f)
            if !b.paintPos.contains(localPos):
                continue
            if !f.hitTest(localPos, editView.zoom):
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
        if selection.count == 0:
            return null
        return cast(*BaseForm) @dataof(forms[selection.back()])
    }

    mtd getSelectedFormType()->typeinfo
    {
        if selection.count == 0:
            return null
        let i = forms[selection.back()]
        return @kindof(i)
    }

    mtd updateEditModeFromSelection()
    {
        if !selection.count:
            return

        let main = editView.main
        if main.editMode == .Selection:
            return

        let f = forms[selection.back()]
        main.updateEditMode(f)
    }

    mtd updateModelFromSelection()
    {
        if !selection.count:
            return

        let f    = forms[selection.back()]
        let t    = @kindof(f)
        let form = cast(*BaseForm) @dataof(f)
        if form.subKind == .Highlight:
            editView.main.mdlHighlight = f
        elif form.subKind == .PixSel:
            editView.main.mdlPixSel = f

        #[Swag.Complete]
        switch t
        {
        case FormShape:
            let b = cast(*FormShape) @dataof(f)
            editView.main.mdlFormShape = dref b

        case FormImage:
            let b = cast(*FormImage) @dataof(f)
            editView.main.mdlFormImage = dref b

        case FormStep:
            let b = cast(*FormStep) @dataof(f)
            editView.main.mdlFormStep    = dref b
            editView.main.mdlFormStep.ws = b.w
            editView.main.mdlFormStep.hs = b.h

        case FormArrow:
            let b = cast(*FormArrow) @dataof(f)
            editView.main.mdlFormArrow = dref b

        case FormLine:
            let b = cast(*FormLine) @dataof(f)
            editView.main.mdlFormLine = dref b

        case FormPolygon:
            let b = cast(*FormPolygon) @dataof(f)
            editView.main.mdlFormPolygon = dref b

        case FormText:
            let b = cast(*FormText) @dataof(f)
            editView.main.mdlFormText = dref b

        case FormDraw:
            let b = cast(*FormDraw) @dataof(f)
            editView.main.mdlFormDraw = dref b
        }
    }

    mtd selectionHasChanged()
    {
        updateEditModeFromSelection()
        updateModelFromSelection()
        editView.main.updateState()
        editView.invalidate()
    }

    //////////////////////////////
    // UNDO
    //////////////////////////////

    mtd newUndo(kind: UndoKind, serial: u32 = 0)->*Undo
    {
        let ptr = undos.newPtr()
        ptr.kind = kind
        if serial:
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
        if !canUndo():
            return

        let serial = undos[nextUndo - 1].serial
        while nextUndo and undos[nextUndo - 1].serial == serial
        {
            nextUndo -= 1
            undos[nextUndo].undoIt()
        }

        editView.invalidate()
    }

    mtd redo()
    {
        if !canRedo():
            return

        let serial = undos[nextUndo].serial
        while nextUndo != undos.count and undos[nextUndo].serial == serial
        {
            undos[nextUndo].doIt()
            nextUndo += 1
        }

        editView.invalidate()
    }

    mtd pushUndo(u: nn *Undo)
    {
        u.capture = self

        // Remove no more relevant undos
        let countUndo = nextUndo
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
        u.doIt()
    }
}
