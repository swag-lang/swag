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