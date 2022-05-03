using Core, Gui, Pixel

struct Capture
{
    // To save
    width, height: s32
    forms:  Array'IForm

    // Dyn
    editView:   *EditView
    gizmo:      GizmoRect
    undos:      Array'(*Undo)
    nextUndo:   uint
    hotForm:    uint = Swag.UInt.Max
    selection:  Array'uint

    moving:             bool
    firstMoving:        bool
    movingCursorSet:    bool
    movingStart:        Math.Point
}

impl Capture
{
    mtd setup(view: *EditView)
    {
        editView = view
        gizmo.sigChanged = @|self|(g, first) {
            undo := Memory.new'Undo()
            undo.capture = self
            undo.kind = .MoveCapture
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

        zoom := rect.width / width

        // Shapes
        app := editView.getApp()
        render := &app.renderer
        painter.pushClippingRect(rect)

        var pt: Painter

        visit f, i: forms
        {
            base := cast(*BaseForm) @dataof(f)
            base.paintPos.x      = 0
            base.paintPos.y      = 0
            base.paintPos.width  = base.w * zoom
            base.paintPos.height = base.h * zoom

            pt.begin()
            pt.setParams(.Antialiased)
            pt.colorMask(true, true, true, true)
            f.paint(&pt, zoom)
            pt.end()

            base.paintPos.x = rect.x + (base.x * zoom)
            base.paintPos.y = rect.y + (base.y * zoom)

            rt := render.getRenderTarget(cast(s32) base.paintPos.width, cast(s32) base.paintPos.height)
            render.bindRenderTarget(rt)
            render.clear()
            render.draw(&pt)
            render.unbindRenderTarget()

            srcRect := Math.Rectangle{0, 0, base.paintPos.width, base.paintPos.height}
            painter.drawTexture(base.paintPos, srcRect, rt.colorBuffer)

            painter.releaseRenderTarget(rt)
        }

        painter.setBlendingMode(.Alpha)
        painter.popClippingRect()

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

    mtd mouse(view: *EditView, evt: *MouseEvent)->bool
    {
        app := view.getApp()

        localPos := view.surfaceToLocal(evt.surfacePos)
        localPos += view.scrollPos

        if !moving
        {
            // Resize capture
            if gizmo.mouse(view, localPos, evt)
            {
                view.invalidate()
                evt.accepted = true
                return true
            }

            ht := hitTest(localPos)
            if ht != hotForm
            {
                view.invalidate()
                hotForm = ht
            }

            // Specific moves only if one single selection
            if selection.count == 1
            {
                selForm := forms[selection[0]]
                if selForm.mouse(view, localPos, evt)
                    return true
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

        // End of selection move
        if moving and evt.kind == .MouseReleased and evt.button == .Left
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
                moving, firstMoving = true
                movingStart = localPos
                view.captureMouse()
                view.invalidate()
            }
        }

        // Move selection
        if moving
        {
            newSizeX := cast(s32) ((localPos.x - movingStart.x) / view.zoom)
            newSizeY := cast(s32) ((localPos.y - movingStart.y) / view.zoom)

            if firstMoving
            {
                if Math.abs(newSizeX) <= 3 and Math.abs(newSizeY) <= 3
                    return true
                firstMoving = false
            }
            else
                undo()

            u := Memory.new'Undo()
            u.kind = .OffsetSelection
            u.selection = selection
            u.capture = self
            u.newSizeX = newSizeX
            u.newSizeY = newSizeY
            pushUndo(u)
            view.invalidate()

            return true
        }

        return false
    }

    mtd hitTest(localPos: Math.Point)->uint
    {
        visit f, i: forms
        {
            b := cast(*BaseForm) @dataof(f)
            if b.paintPos.contains(localPos)
                return i
        }

        return Swag.UInt.Max
    }

    //////////////////////////////
    // UNDO
    //////////////////////////////

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
        u.do()
    }
}