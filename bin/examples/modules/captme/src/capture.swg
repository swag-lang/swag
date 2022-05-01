using Core, Gui, Pixel

struct Capture
{
    // To save
    width, height: s32
    forms:  Array'IForm

    // Dyn
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
    //////////////////////////////
    // PAINT
    //////////////////////////////

    mtd paint(painter: *Painter, rect: Math.Rectangle)
    {
        painter.fillRect(rect, Argb.Gray)
        painter.fillRect(rect, Brush.createHatch(.CheckBoardLarge, Argb.DarkGray))
        painter.pushClippingRect(rect)

        zoom := rect.width / width

        painter.setParams(.Antialiased)
        visit f, i: forms
        {
            flags := FormFlags.Zero
            if i == hotForm
                flags |= .Hot
            if selection.contains(i)
                flags |= .Selected
            f.paint(painter, rect.x, rect.y, zoom, flags)
        }

        painter.popClippingRect()
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
            newSizeX := cast(s32) (localPos.x - movingStart.x)
            newSizeY := cast(s32) (localPos.y - movingStart.y)

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

        // Resize capture
        if gizmo.mouse(view, localPos, evt)
        {
            view.invalidate()
            evt.accepted = true
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