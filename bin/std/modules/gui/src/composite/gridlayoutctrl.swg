#global public
using Core

struct GridLayoutCtrl
{
    using frameWnd: FrameWnd
    numColumns:     s32
    numRows:        s32
    spacing:        f32 = 5
    layout:         Array'*Wnd
    colSizes:       Array's32
    rowSizes:       Array's32
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
                defer x += colSizes[i]

                idx := i + (j * numColumns)
                curWnd := layout[idx]
                if !curWnd or done.contains(curWnd)
                    continue

                done.add(curWnd)

                w := 0
                h := 0

                idxC := i
                idxW := idx
                while idxC < numColumns and layout[idxW] == curWnd
                {
                    w += colSizes[idxC]
                    idxW, idxC += 1
                }

                idxC  = j
                idxH := idx
                while idxC < numRows and layout[idxH] == curWnd
                {
                    h += rowSizes[idxC]
                    idxH += numColumns
                    idxC += 1
                }

                var rect: Math.Rectangle
                rect.x = x
                rect.y = y
                rect.width = w
                rect.height = h
                curWnd.setPosition(rect)
            }

            y += rowSizes[j]
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