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
        lastSelectPos:         Math.Point
        lastSelectHit:         u64
        firstMouseMove:        bool
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
        changed = true
    }

    mtd toImage(zoom: f32 = 1, onlySel = false)->Image
    {
        var result: retval

        var rect     = Math.Rectangle{width: width * zoom, height: height * zoom}
        var painter: Pixel.Painter

        let app    = editView.getApp()
        let render = app.getRenderer()

        with painter
        {
            .begin()
            .setColorMaskFull()
            .clear(Argb.White)
            paintBackImage(&painter, 0, 0, zoom)
            .setColorMaskColor()
            .setParams(.Antialiased)
        }

        if onlySel
        {
            foreach sel in selection
            {
                let f = forms[sel]
                let b = cast(*BaseForm) @dataof(f)
                if b.subKind == .PixSel:
                    continue
                paintForm(app, &painter, {}, rect, f, zoom, snapMode: true)
            }
        }
        else
        {
            foreach f in forms
            {
                let b = cast(*BaseForm) @dataof(f)
                if b.subKind == .PixSel:
                    continue
                paintForm(app, &painter, {}, rect, f, zoom, snapMode: true)
            }
        }

        painter.end()

        render.begin(editView.surface.rc)

        let rt = render.getRenderTarget(cast(s32) rect.width, cast(s32) rect.height)
        with render
        {
            .bindRenderTarget(rt)
            .setScissorRect(rect)
            .clear()
            .draw(&painter)
            .unbindRenderTarget()
            .releaseRenderTarget(rt)
        }

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
        editView.invalidate()
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

    mtd cut()
    {
        copy()
        pushUndo(newUndo(.DeleteSelection))
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

    func paintForm(app: *Application, painter: *Painter, clip, rect: Math.Rectangle, f: IForm, zoom: f32, quickMode = false, snapMode = false)
    {
        let render = &app.renderer

        var boundRect = f.boundRect()
        boundRect.width  = Math.max(boundRect.width, 1)
        boundRect.height = Math.max(boundRect.height, 1)
        boundRect.inflate(200)

        var clipOffset: Math.Vector2
        with let base = cast(*BaseForm) @dataof(f)
        {
            var savePaintPos = .paintPos

            .paintPos.x      = rect.x + (boundRect.x * zoom)
            .paintPos.y      = rect.y + (boundRect.y * zoom)
            .paintPos.width  = boundRect.width * zoom
            .paintPos.height = boundRect.height * zoom

            var saveNoClipPos = .paintPos

            if !quickMode and !clip.isEmpty()
            {
                if .paintPos.x >= clip.right():
                    return
                if .paintPos.y >= clip.bottom():
                    return
                if .paintPos.x < clip.x:
                    clipOffset.x = clip.x - .paintPos.x
                if .paintPos.y < clip.y:
                    clipOffset.y = clip.y - .paintPos.y
                if .paintPos.right() > clip.right():
                    .paintPos.setRight(clip.right())
                if .paintPos.bottom() > clip.bottom():
                    .paintPos.setBottom(clip.bottom())
            }

            .paintPos.width += .paintPos.x - Math.round(.paintPos.x)
            .paintPos.height += .paintPos.y - Math.round(.paintPos.y)
            .paintPos.round()

            let wt2 = cast(s32) (.paintPos.width - clipOffset.x)
            let ht2 = cast(s32) (.paintPos.height - clipOffset.y)
            var wt  = wt2
            var ht  = ht2
        }

        let rt = render.getRenderTarget(wt2, ht2)
        with painter
        {
            .bindRenderTarget(rt, true)
            .setAntialiased()
            .setColorMaskFull()
            .clear(Argb.Zero)
            .setColorMaskColor()
            .setBlendingMode(.Alpha)
            .pushTransform()
            .translateTransform((-boundRect.x * zoom) - clipOffset.x, (-boundRect.y * zoom) - clipOffset.y)
            var cxt: FormPaintContext
            cxt.z         = zoom
            cxt.quickMode = quickMode
            f.paint(painter, cxt)
            .popTransform()
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
            var srcRect = Math.Rectangle{0, 0, wt2, ht2}
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
                .width  = wt2
                .height = ht2
                .offset(-base.shadowOffsetX, -base.shadowOffsetY)
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

        var srcRect = Math.Rectangle{0, 0, wt, ht}

        var paintRect: Math.Rectangle
        paintRect.x      = base.paintPos.x + clipOffset.x
        paintRect.y      = base.paintPos.y + clipOffset.y
        paintRect.width  = wt
        paintRect.height = ht

        if base.paintShadow:
            paintRect.offset(base.shadowOffsetX - Margin, base.shadowOffsetY - Margin)

        // Rotation
        painter.pushTransform()

        if base.subKind != .Highlight and base.subKind != .PixSel and !Math.isZeroEpsilon(base.rotate)
        {
            painter.translateTransform(saveNoClipPos.horzCenter(), saveNoClipPos.vertCenter())
            paintRect.offset(-saveNoClipPos.horzCenter(), -saveNoClipPos.vertCenter())

            let angle = Math.map(base.rotate, -180, 180, -Math.ConstF32.Pi, Math.ConstF32.Pi)
            painter.rotateTransformInPlace(angle)
        }

        defer painter.popTransform()

        if base.paintShadow
        {
            with painter
            {
                .setColorMaskColor()
                .drawTexture(paintRect, srcRect, rt1.colorBuffer, colForm)
                .setBlendingMode(.Max)
                .setColorMaskAlpha()
                .drawTexture(paintRect, srcRect, rt1.colorBuffer, colForm)
            }
        }
        else
        {
            with painter
            {
                .setColorMaskColor()
                .setInterpolationMode(.Pixel)
                .drawTexture(paintRect, srcRect, rt.colorBuffer, colForm)
                .setBlendingMode(.Max)
                .setColorMaskAlpha()
                .drawTexture(paintRect, srcRect, rt.colorBuffer, colForm)
                .setInterpolationMode(.Linear)
            }
        }

        with painter
        {
            .setColorMaskColor()
            .setBlendingMode(.Alpha)
            .releaseRenderTarget(rt)
            .releaseRenderTarget(rt1)
            .releaseRenderTarget(rt2)
        }

        if snapMode:
            base.paintPos = savePaintPos
    }

    mtd paintBackImage(painter: *Painter, x, y: f32, z: f32)
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

    mtd paint(painter: *Painter, clip, rect: Math.Rectangle)
    {
        painter.fillRect(rect, Argb.LightGray)
        painter.fillRect(rect, Brush.createHatch(.CheckBoardLarge, Argb.DarkGray))
        painter.setParams(.Antialiased)

        paintRect = rect
        let zoom = rect.width / width

        // Background image
        painter.setAntialiased(false)
        paintBackImage(painter, rect.x, rect.y, zoom)

        // Shapes
        let app = editView.getApp()
        foreach f in forms
        {
            var pp = f.boundRect() * zoom
            if !pp.intersectWith(clip):
                continue

            if f.mustClipToBound()
            {
                pp.offset(rect.x, rect.y)
                painter.pushClippingRect(pp)            
                paintForm(app, painter, clip, rect, f, zoom)
                painter.popClippingRect()
            }
            else
            {      
                paintForm(app, painter, clip, rect, f, zoom)          
            }
        }

        // Gizmos
        foreach f, i in forms
        {
            var flags = FormFlags.Zero
            if i == hotForm:
                flags.add(.Hot)
            if selection.contains(i):
                flags.add(.Selected)

            painter.pushTransform()
            painter.translateTransform(rect.x, rect.y)
            f.paintGizmo(painter, zoom, flags)
            painter.popTransform()
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

            r.offset(paintRect.x, paintRect.y)
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

    mtd snap(x, y: nl *s32, right, bottom: bool, idxPt: u32 = 0, editForm: IForm = null)
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

    mtd popupMenu(view: *EditView, pos: Math.Point)
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
            .addItem(CmdIdCut)
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
            .addItem(CmdIdSendToFront)
            .addItem(CmdIdSendToBack)
            .addItem(CmdIdMoveForward)
            .addItem(CmdIdMoveBackward)
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
        let app = view.getApp()

        var localPos = view.surfaceToLocal(evt.surfacePos)
        localPos += view.scrollPos
        localPos.x -= paintRect.x
        localPos.y -= paintRect.y

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
                let p       = cast(*BaseForm) @dataof(selForm)
                if (!view.main.freezeSel or !p.freeze) and selForm.mouse(view, localPos, evt)
                {
                    evt.accepted = true
                    if evt.kind != .MouseLeave:
                        return true
                }
            }

            // Popup
            if evt.kind == .MousePressed and evt.button == .Right
            {
                popupMenu(view, evt.surfacePos)
                return true
            }

            if evt.kind == .MouseLeave
            {
                hotForm = Swag.U64.Max
            }
            else
            {
                var ht = hitTest(localPos)
                if ht != Swag.U64.Max
                {
                    let p = cast(*BaseForm) @dataof(forms[ht])
                    if view.main.freezeSel and p.freeze:
                        ht = Swag.U64.Max
                }

                if ht != hotForm
                {
                    view.invalidate()
                    hotForm = ht
                }
            }
        }

        if hotForm != Swag.U64.Max:
            app.setFrameCursor(Cursor.from(.SizeAll))

        let shiftPressed = Input.Keyboard.isKeyPressed(.Shift) or Input.Keyboard.isKeyPressed(.RShift)
        let ctrlPressed  = Input.Keyboard.isKeyPressed(.Control) or Input.Keyboard.isKeyPressed(.RControl)

        // Start of mode
        ///////////////////////
        if mouseMode == .None and evt.button == .Left
        {
            if evt.kind == .MousePressed or evt.kind == .MouseDoubleClick
            {
                // Ctrl+Shift force the selection mode
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
                    var here         = selection.contains(hotForm)

                    // Swap between objects at the same position if no mouse move
                    if lastSelectPos.isEqualEpsilon(evt.surfacePos)
                    {
                        if !evt.modifiers.has(.Control) and selection.count
                        {
                            selection.clear()
                            selectionHasChanged()
                        }

                        let res = hitTests(localPos, false)
                        lastSelectHit += 1
                        if lastSelectHit >= res.count:
                            lastSelectHit = 0
                        hotForm = res[lastSelectHit]
                        here    = selection.contains(hotForm)
                    }
                    elif !evt.modifiers.has(.Control) and !here and selection.count
                    {
                        selection.clear()
                        selectionHasChanged()
                        lastSelectHit = 0
                    }

                    if hotForm != Swag.U64.Max
                    {
                        if !here
                        {
                            lastSelectPos = evt.surfacePos
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
                let pp = f.boundRect() * view.zoom
                if pp.intersectWith(r) and !selection.contains(idx):
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

                let mode = editView.main.getEditMode()
                let itf  = editView.main.newFormFromModel(mode, clear: true)
                if itf:
                    itf.setup(self)
                let undo = newUndo(.CreateForm)
                undo.oldForms.add(itf)
                pushUndo(undo)
            }

            let f = forms.back()
            if !f.edit(evt, movingStart, localPos, view.zoom)
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
        let res = hitTests(localPos, true)
        if res.isEmpty():
            return Swag.U64.Max
        return res[0]
    }

    mtd hitTests(localPos: Math.Point, retFirst: bool)->Array'u64
    {
        var result: retval
        if !forms.count:
            return result
        for #back i in forms
        {
            let f = forms[i]
            if !f.hitTest(localPos, editView.zoom):
                continue
            result.add(i)
            if retFirst:
                break
        }

        return result
    }

    //////////////////////////////
    // SELECTION
    //////////////////////////////

    mtd getSelectedBaseForm()->nl *BaseForm
    {
        if selection.count == 0:
            return null
        return cast(*BaseForm) @dataof(forms[selection.back()])
    }

    mtd getSelectedFormType()->nl typeinfo
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

        let f = forms[selection.back()]
        editView.main.updateModel(f)
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

    mtd pushUndo(u: *Undo)
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
