using Serialization

const PreviewHeight = 256.0
const PreviewWidth  = PreviewHeight * (4/3.0)

enum MouseMode
{
    None
    Moving
    Creating
}

struct Capture
{
    // To save
    width, height:          s32
    backImg:                Image
    widthOrg, heightOrg:    s32
    backImgOrg:             Image

    backImagePos:   Math.Point
    preview:        Image
    file:           String
    forms:          Array'IForm

    // Dyn
    #[NoSerialize]
    {
        changed:    bool
        mouseMode:  MouseMode = None
        editView:   *EditView
        gizmo:      GizmoRect
        undos:      ArrayPtr'Undo
        nextUndo:   uint
        hotForm:    uint = Swag.UInt.Max
        selection:  Array'uint
        paintRect:  Math.Rectangle

        backTexture: Texture
        snapX:       f32 = -1
        snapY:       f32 = -1

        firstMouseMove:     bool
        movingCursorSet:    bool
        movingStart:        Math.Point
        dirtyBackImg:       bool
    }
}

impl Capture
{
    mtd setup(view: *EditView)
    {
        editView = view
        gizmo.sigChanged = @|self|(g, first, hitMove) {
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
        painter.setColorMask(false, false, false, true)
        painter.clear(Argb.Zero)
        painter.setColorMask(true, true, true, true)
        paintBackImage(&painter, 0, 0, zoom)
        painter.setColorMask(true, true, true, false)
        painter.setParams(.Antialiased)

        if onlySel
        {
            visit sel: selection
            {
                f := forms[sel]
                paintForm(app, &painter, rect, f, zoom, snapMode: true)
            }
        }
        else
        {
            visit f: forms
                paintForm(app, &painter, rect, f, zoom, snapMode: true)
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
        editView.main.recentWnd.updateRecent(self)
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
        visit s: selection
            arr.add(forms[s])

        var buf:    ConcatBuffer
        var encode: Encoder'Write.TagBin

        catch
        {
            encode.start(&buf)
            if @err return
            encode.writeValue(&arr, @typeof(arr))
            if @err return
            encode.end()
            if @err return
        }

        buf.makeLinear()
        Clipboard.addData(editView.main.clipFmt, buf.toSlice())
    }

    mtd paste()
    {
        var decode: Decoder'Read.TagBin
        var res:    Array'IForm

        arr := Clipboard.getData(editView.main.clipFmt)
        if arr.count
        {
            catch decode.start(arr.toSlice())
            if @err return
            catch decode.readValue(&res, Array'IForm)
            if @err or !res.count return
            catch decode.end()
            if @err return
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
            if idx == 0
                boundRect = itf.boundRect(false)
            else
                boundRect.setUnion(itf.boundRect(false))
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
            bound := itf.boundRect(false)
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
        base := cast(*BaseForm) @dataof(f)

        savePaintPos := base.paintPos
        base.paintPos.x      = rect.x + (boundRect.x * zoom)
        base.paintPos.y      = rect.y + (boundRect.y * zoom)
        base.paintPos.width  = boundRect.width * zoom
        base.paintPos.height = boundRect.height * zoom

        rt := render.getRenderTarget(cast(s32) base.paintPos.width, cast(s32) base.paintPos.height)
        painter.bindRenderTarget(rt, true)
        painter.setParams(.Antialiased)
        painter.clear(Argb.Zero)
        f.paint(app, painter, zoom, quickMode)
        painter.unbindRenderTarget()

        const Margin = 8
        wt2 := cast(s32) base.paintPos.width
        ht2 := cast(s32) base.paintPos.height
        wt  := wt2 + 2*Margin
        ht  := ht2 + 2*Margin
        rt1 := render.getRenderTarget(wt, ht)
        rt2 := render.getRenderTarget(wt, ht)

        // Draw the image with a margin around
        srcRect := Math.Rectangle{0, 0, base.paintPos.width, base.paintPos.height}
        painter.bindRenderTarget(rt1)
        painter.setShader(&render.shaderSimple)
        painter.setBlendingMode(.Copy)
        painter.setColorMask(true, true, true, true);
        painter.clear(Color.fromArgb(0,0,0,0))
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
        painter.setColorMask(true, true, true, true);
        painter.clear(Color.fromArgb(0,0,0,0))
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
        painter.setColorMask(true, true, true, true);
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

        // Paint resulting image
        painter.pushClippingRect(rect)
        painter.setShader()

        paintShadowRect := dstRect
        paintShadowRect.x = base.paintPos.x - Margin
        paintShadowRect.y = base.paintPos.y - Margin
        paintShadowRect.offset(base.shadowOffset.x, base.shadowOffset.y)

        var colForm: Color
        if quickMode and (@kindof(f) == FormShape or @kindof(f) == FormDraw)
            colForm = Argb.White
        else
            colForm = Color.fromArgb(cast(u8) Math.map(cast(f32) base.alpha, 0, 100, 0, 255), Argb.White)

        painter.drawTexture(paintShadowRect, dstRect, rt1.colorBuffer, colForm)
        painter.popClippingRect()

        painter.releaseRenderTarget(rt)
        painter.releaseRenderTarget(rt1)
        painter.releaseRenderTarget(rt2)

        if snapMode
            base.paintPos = savePaintPos
    }

    mtd paintBackImage(painter: *Painter, x, y: f32, z: f32)
    {
        renderer := editView.getApp().renderer

        // Background image
        if !backTexture.isValid() and backImg.width > 0
            backTexture = renderer.addImage(backImg)

        if backTexture.isValid()
        {
            // Background image has been changed
            if dirtyBackImg
            {
                if backTexture.pf != backImg.pf
                {
                    renderer.removeTexture(&backTexture)
                    backTexture = renderer.addImage(backImg)
                }
                else
                    renderer.updateTexture(&backTexture, backImg.pixels.buffer)

                dirtyBackImg = false
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

        // Snap
        if snapX != -1
            painter.drawLine(rect.x + snapX * zoom, rect.y, rect.x + snapX * zoom, rect.bottom(), Argb.Purple, 2)
        if snapY != -1
            painter.drawLine(rect.x, rect.y + snapY * zoom, rect.right(), rect.y + snapY * zoom, Argb.Purple, 2)
    }

    //////////////////////////////
    // SNAP
    //////////////////////////////

    mtd getSnapPoint(bound: Math.Rectangle, right, bottom: bool, nx, ny: s32, idxPt: u32)->Math.Point
    {
        var result: retval

        form := forms[selection[0]]
        type := @kindof(form)

        if selection.count == 1 and type == FormPolygon
        {
            poly := cast(*FormPolygon) @dataof(form)
            result = dref poly.points[idxPt]
            result.x += nx
            result.y += ny
        }
        elif selection.count == 1 and (type == FormLine or type == FormArrow)
        {
            line := cast(*FormLine) @dataof(form)
            if idxPt == 0
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

    mtd getSelectionBoundRect()->Math.Rectangle
    {
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

    mtd snap(x, y: *s32, right, bottom: bool, idxPt: u32 = 0)
    {
        const SnapToler = 10

        if x
            snapX = -1
        if y
            snapY = -1

        if !editView.main.snap
            return

        nx := x ? dref x : 0
        ny := y ? dref y : 0

        bound := getSelectionBoundRect()

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
            if selection.contains(i)
                continue

            form := cast(*BaseForm) @dataof(f)

            switch @kindof(f)
            {
            case FormLine, FormArrow:
                form2 := cast(*FormLine) form
                arrX.add(cast(s32) form2.xa)
                arrX.add(cast(s32) form2.xb)
                arrY.add(cast(s32) form2.ya)
                arrY.add(cast(s32) form2.yb)

            case FormPolygon:
                form2 := cast(*FormPolygon) form
                visit pt: form2.points
                {
                    arrX.add(cast(s32) pt.x)
                    arrY.add(cast(s32) pt.y)
                }

            case FormDraw:
                form2 := cast(*FormDraw) form
                bound := f.boundRect(false)
                arrX.add(cast(s32) bound.x)
                arrX.add(cast(s32) bound.right())
                arrY.add(cast(s32) bound.y)
                arrY.add(cast(s32) bound.bottom())

            default:
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
                p   := getSnapPoint(bound, right, bottom, nx, ny, idxPt)
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
                p   := getSnapPoint(bound, right, bottom, nx, ny, idxPt)
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
            p := getSnapPoint(bound, right, bottom, 0, 0, idxPt)
            nx = cast(s32) (bestCoordX - p.x)
        }

        if y and bestY < SnapToler
        {
            snapY = bestCoordY
            p := getSnapPoint(bound, right, bottom, 0, 0, idxPt)
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
        menu := MenuCtrl.createPopup(view)

        menu.addItem(CmdIdCopy)
        menu.addItem(CmdIdPaste)
        menu.addSeparator()
        menu.addItem(CmdIdDeleteSelection)
        menu.addSeparator()

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

        menu.addSeparator()
        menu.addItem(CmdIdCopyAsImage)
        menu.addItem(CmdIdSelectAll)
        menu.addItem(CmdIdDeSelectAll)
        menu.addSeparator()
        menu.addItem(CmdIdFlatten)
        menu.addItem(CmdIdFlattenAll)
        menu.addSeparator()
        menu.addItem(CmdIdSnap)

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

            if evt.kind == .MouseLeave
            {
                hotForm = Swag.UInt.Max
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

            if hotForm != Swag.UInt.Max
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

        // Start of mode
        ///////////////////////
        if mouseMode == .None and evt.button == .Left
        {
            if evt.kind == .MousePressed or evt.kind == .MouseDoubleClick
            {
                here := selection.contains(hotForm)
                if !(evt.modifiers & .Control) and !here
                {
                    if selection.count
                    {
                        selection.clear()
                        selectionHasChanged()
                    }
                }

                if hotForm != Swag.UInt.Max
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
                else
                {
                    mouseMode = .Creating
                    firstMouseMove = true
                    movingStart = localPos
                    view.captureMouse()
                    view.invalidate()
                }
            }
        }

        // End of mode
        ///////////////////////
        if evt.kind == .MouseReleased and evt.button == .Left
        {
            snapX, snapY = -1

            if mouseMode == .Moving and hotForm == Swag.UInt.Max
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
                if Math.abs(newPosX) <= 3 and Math.abs(newPosY) <= 3
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
                case Favorites, Highlight:
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

    mtd hitTest(localPos: Math.Point)->uint
    {
        for i := cast(s32) forms.count - 1; i >= 0; i -= 1
        {
            f := forms[i]
            b := cast(*BaseForm) @dataof(f)
            if !b.paintPos.contains(localPos)
                continue
            if !f.hitTest(localPos, self.editView.zoom)
                continue
            return i
        }

        return Swag.UInt.Max
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

        f := forms[selection.back()]
        t := @kindof(f)
        main := editView.main

        if main.editMode == .Favorites
        {
            #[Swag.Complete]
            switch t
            {
            case FormShape, FormImage:
                main.mdlFavorite = main.mdlFormShape
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
        elif main.editMode == .Highlight
        {
            #[Swag.Complete]
            switch t
            {
            case FormShape, FormImage:
                main.mdlHighlight = main.mdlFormShape
            case FormArrow:
                main.mdlHighlight = main.mdlFormArrow
            case FormLine:
                main.mdlHighlight = main.mdlFormLine
            case FormPolygon:
                main.mdlHighlight = main.mdlFormPolygon
            case FormText:
                main.mdlHighlight = main.mdlFormText
            case FormDraw:
                main.mdlHighlight = main.mdlFormDraw
            }
        }
        else
        {
            #[Swag.Complete]
            switch t
            {
            case FormShape, FormImage:
                main.editMode = .Shape
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

        #[Swag.Complete]
        switch t
        {
        case FormShape:
            b := cast(*FormShape) @dataof(f)
            editView.main.mdlFormShape = dref b

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

        case FormImage:
            break
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

    mtd newUndo(kind: UndoKind)->*Undo
    {
        ptr := undos.newPtr()
        ptr.kind = kind
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
        if !canUndo() return
        nextUndo -= 1
        undos[nextUndo].undo()
        editView.invalidate()
    }

    mtd redo()
    {
        if !canRedo() return
        undos[nextUndo].do()
        nextUndo += 1
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