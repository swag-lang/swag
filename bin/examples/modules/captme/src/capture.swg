using Core, Gui, Pixel

struct Capture
{
    // To save
    width, height: s32
    forms:  Array'IForm

    // Dyn
    editView:   *EditView
    gizmo:      GizmoRect
    undos:      ArrayPtr'Undo
    nextUndo:   uint
    hotForm:    uint = Swag.UInt.Max
    selection:  Array'uint
    paintRect:  Math.Rectangle

    moving:             bool
    creating:           bool
    firstMouseMove:        bool
    movingCursorSet:    bool
    movingStart:        Math.Point
}

impl Capture
{
    mtd setup(view: *EditView)
    {
        editView = view
        gizmo.sigChanged = @|self|(g, first) {
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
    // PAINT
    //////////////////////////////

    mtd paint(painter: *Painter, rect: Math.Rectangle)
    {
        painter.fillRect(rect, Argb.Gray)
        painter.fillRect(rect, Brush.createHatch(.CheckBoardLarge, Argb.DarkGray))
        painter.setParams(.Antialiased)

        paintRect = rect
        zoom := rect.width / width

        // Shapes
        app := editView.getApp()
        render := &app.renderer

        visit f: forms
        {
            boundRect := f.boundRect()
            base := cast(*BaseForm) @dataof(f)
            base.paintPos.x      = rect.x + (boundRect.x * zoom)
            base.paintPos.y      = rect.y + (boundRect.y * zoom)
            base.paintPos.width  = boundRect.width * zoom
            base.paintPos.height = boundRect.height * zoom

            rt := render.getRenderTarget(cast(s32) base.paintPos.width, cast(s32) base.paintPos.height)
            painter.bindRenderTarget(rt, true)
            painter.setParams(.Antialiased)
            painter.clear(Argb.Zero)
            f.paint(painter, zoom)
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

            // Blur first pass
            painter.bindRenderTarget(rt2)
            painter.setShader(&render.shaderBlur)
            painter.setShaderParam(render.shaderBlur.axis, 0's32)
            painter.setShaderParam(render.shaderBlur.radius, 8.0)
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
            painter.setShaderParam(render.shaderBlur.radius, 8.0)
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
            colForm := Color.fromArgb(cast(u8) Math.map(cast(f32) base.alpha, 0, 100, 0, 255), Argb.White)
            painter.drawTexture(paintShadowRect, dstRect, rt1.colorBuffer, colForm)
            painter.popClippingRect()

            painter.releaseRenderTarget(rt)
            painter.releaseRenderTarget(rt1)
            painter.releaseRenderTarget(rt2)
        }

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
    }

    //////////////////////////////
    // CONTENT
    //////////////////////////////

    mtd popupMenu(view: *EditView, pos: Math.Point)
    {
        menu := MenuCtrl.createPopup(view)

        {
            popup := MenuCtrl.createPopup(view)
            popup.addItem(CmdIdAlignLeft)
            popup.addItem(CmdIdAlignCenter)
            popup.addItem(CmdIdAlignRight)
            popup.addSeparator()
            popup.addItem(CmdIdAlignTop)
            popup.addItem(CmdIdAlignMiddle)
            popup.addItem(CmdIdAlignBottom)
            menu.addPopup("Align", popup)
        }

        {
            popup := MenuCtrl.createPopup(view)
            popup.addItem(CmdIdBringToFront)
            popup.addItem(CmdIdSendToBack)
            popup.addItem(CmdIdBringForward)
            popup.addItem(CmdIdSendBackward)
            menu.addPopup("Arrange", popup)
        }

        discard menu.doModal(pos)
        view.setFocus()
    }

    mtd mouse(view: *EditView, evt: *MouseEvent)->bool
    {
        app := view.getApp()

        localPos := view.surfaceToLocal(evt.surfacePos)
        localPos += view.scrollPos

        if !moving and !creating
        {
            if evt.kind == .MouseLeave
                return true

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
            if gizmo.mouse(view, localPos, evt)
            {
                view.invalidate()
                evt.accepted = true
                return true
            }

            // Specific moves only if one single selection
            if selection.count == 1
            {
                selForm := forms[selection[0]]
                if selForm.mouse(view, localPos, evt)
                    return true
            }

            ht := hitTest(localPos)
            if ht != hotForm
            {
                view.invalidate()
                hotForm = ht
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

        if evt.kind == .MouseReleased and evt.button == .Left
        {
            // End of selection move
            if moving
            {
                if hotForm == Swag.UInt.Max
                {
                    app.popCursor()
                    movingCursorSet = false
                }

                view.releaseMouse()
                view.invalidate()
                moving = false
            }

            // End of tool creation
            elif creating
            {
                view.releaseMouse()
                view.invalidate()
                creating = false
            }
        }

        // Selection
        if evt.kind == .MousePressed and evt.button == .Left
        {
            here := selection.contains(hotForm)
            if !(evt.modifiers & .Control) and !here
                selection.clear()

            if hotForm != Swag.UInt.Max
            {
                if !here
                    selection.add(hotForm)
                moving, firstMouseMove = true
                movingStart = localPos
                view.captureMouse()
                view.invalidate()
            }
            else
            {
                creating, firstMouseMove = true
                movingStart = localPos
                view.captureMouse()
                view.invalidate()
            }
        }

        // Move selection
        if moving
        {
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
            u.newPosX = newPosX
            u.newPosY = newPosY
            pushUndo(u)
            view.invalidate()

            return true
        }

        // Create
        if creating
        {
            newPosX := cast(s32) ((localPos.x - movingStart.x) / view.zoom)
            newPosY := cast(s32) ((localPos.y - movingStart.y) / view.zoom)

            if firstMouseMove
            {
                if Math.abs(newPosX) <= 3 and Math.abs(newPosY) <= 3
                    return true
                firstMouseMove = false

                forma := Memory.new'FormRect()
                forma.x = acast ((movingStart.x - paintRect.x) / view.zoom)
                forma.y = acast ((movingStart.y - paintRect.y) / view.zoom)
                forma.w = 0
                forma.h = 0
                forma.setup(self)
                forms.add(cast(IForm) forma)
            }

            f := forms.back()
            b := cast(*BaseForm) @dataof(f)
            b.w, b.h = 0
            f.moveBaseForm(0, 0, Math.max(newPosX, 2), Math.max(newPosY, 2))
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
    }

    mtd redo()
    {
        if !canRedo() return
        undos[nextUndo].do()
        nextUndo += 1
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