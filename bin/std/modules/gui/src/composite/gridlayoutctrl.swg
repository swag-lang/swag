#global public
using Core

struct GridLayoutCtrl
{
    using frameWnd: FrameWnd
    numColumns:     s32
    numRows:        s32
    spacingHorz:    f32 = 5
    spacingVert:    f32 = 5
    layout:         Array'*Wnd
    colSizes:       Array'f32
    rowSizes:       Array'f32
}

impl IWnd for GridLayoutCtrl
{
    mtd onResizeEvent(evt: *ResizeEvent)
    {
        if !childs.count
        {
            frameWnd.IWnd.onResizeEvent(evt)
            return
        }

        var done: Array'(*Wnd)
        y := padding.y
        loop j: numRows
        {
            x := padding.x
            loop i: numColumns
            {
                defer x += colSizes[i] + spacingHorz

                idx := i + (j * numColumns)
                curWnd := layout[idx]
                if !curWnd or done.contains(curWnd)
                    continue

                done.add(curWnd)

                w := 0'f32
                h := 0'f32

                idxC := i
                idxW := idx
                idxN := 0
                while idxC < numColumns and layout[idxW] == curWnd
                {
                    w += colSizes[idxC]
                    idxW, idxC, idxN += 1
                }

                w += (idxN - 1) * spacingHorz

                idxC = j
                idxH := idx
                idxN = 0
                while idxC < numRows and layout[idxH] == curWnd
                {
                    h += rowSizes[idxC]
                    idxH += numColumns
                    idxC, idxN += 1
                }

                h += (idxN - 1) * spacingVert

                var rect: Math.Rectangle
                rect.x = x
                rect.y = y
                rect.width = w
                rect.height = h
                curWnd.setPosition(rect)
            }

            y += rowSizes[j] + spacingVert
        }

        frameWnd.IWnd.onResizeEvent(evt)
    }
}

impl GridLayoutCtrl
{
    // Create a layout control
    func create(parent: *Wnd, numColumns, numRows: s32, position: Math.Rectangle = @{})->*GridLayoutCtrl
    {
        Debug.assert(numColumns > 0 and numRows > 0)

        res := Wnd.create'GridLayoutCtrl(parent, position)
        res.numColumns = numColumns
        res.numRows = numRows

        res.layout.resize(cast(uint) (numColumns * numRows))
        loop numColumns res.colSizes.add(50)
        loop numRows    res.rowSizes.add(50)

        return res
    }

    // Recompute layout of all childs
    mtd computeLayout()
    {
        sendResizeEvent()
    }

    // Set size, in pixel, of a given column
    mtd setColSize(col: s32, size: f32)
    {
        colSizes[col] = size
    }

    // Set size, in pixel, of a given row
    mtd setRowSize(row: s32, size: f32)
    {
        rowSizes[row] = size
    }

    mtd setColRowChild(child: *Wnd, col, row: s32, spanH, spanV: s32 = 1)
    {
        Debug.assert(child.parent == self)
        Debug.assert(col + spanH <= numColumns and row + spanV <= numRows)
        Debug.assert(spanH and spanV)

        loop i: spanH
        {
            x := col + (row * numColumns)
            x += i
            loop j: spanV
            {
                x1 := x + (j * numColumns)
                layout[x1] = child
            }
        }
    }
}