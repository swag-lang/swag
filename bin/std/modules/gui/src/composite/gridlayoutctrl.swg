#global marked
#global public
using Core

#[Swag.EnumFlags]
enum GridLayoutCtrlFlags
{
    Zero               = 0
    AdaptSizeToContent
    AdaptColWidth
    AdaptRowHeight
}

struct GridLayoutCtrl
{
    using wnd:          Wnd

    gridLayoutFlags     = GridLayoutCtrlFlags.Zero
    numColumns:         s32
    numRows:            s32
    spacingHorz:        f32 = 5
    spacingVert:        f32 = 5

    layout:             Array'*Wnd
    colSizes:           Array'f32
    rowSizes:           Array'f32
}

impl IWnd for GridLayoutCtrl
{
    mtd impl onResizeEvent(evt: *ResizeEvent)
    {
        if !me.childs.count
        {
            me.wnd.IWnd.onResizeEvent(evt)
            return
        }

        // Auto compute width of each cell
        var forcedW = 0'f32
        if me.gridLayoutFlags.has(.AdaptColWidth)
        {
            forcedW = me.position.width
            forcedW -= (me.padding.x + me.padding.z)
            forcedW -= (me.numColumns - 1) * me.spacingHorz
            forcedW /= me.numColumns
        }

        // Auto compute height of each cell
        var forcedH = 0'f32
        if me.gridLayoutFlags.has(.AdaptRowHeight)
        {
            forcedH = me.position.height
            forcedH -= (me.padding.y + me.padding.w)
            forcedH -= (me.numRows - 1) * me.spacingVert
            forcedH /= me.numRows
        }

        var done: Array'(*Wnd)
        var maxW, maxH = 0'f32
        var y = me.padding.y
        for j in me.numRows
        {
            var x = me.padding.x
            for i in me.numColumns
            {
                let colW = forcedW orelse me.colSizes[i]
                defer x += colW + me.spacingHorz

                let idx    = i + (j * me.numColumns)
                let curWnd = me.layout[idx]
                if !curWnd or done.contains(curWnd):
                    continue

                done.add(curWnd)

                var w = 0'f32
                var h = 0'f32

                var idxC = i
                var idxW = idx
                var idxN = 0
                while idxC < me.numColumns and me.layout[idxW] == curWnd
                {
                    w += forcedW orelse me.colSizes[idxC]
                    idxW, idxC, idxN += 1
                }

                w += (idxN - 1) * me.spacingHorz

                idxC = j
                idxW = idx
                idxN = 0
                while idxC < me.numRows and me.layout[idxW] == curWnd
                {
                    h += forcedH orelse me.rowSizes[idxC]
                    idxW += me.numColumns
                    idxC, idxN += 1
                }

                h += (idxN - 1) * me.spacingVert

                curWnd.setPosition(Math.Rectangle{x, y, w, h})
                maxW = Math.max(maxW, x + w + me.padding.z)
                maxH = Math.max(maxH, y + h + me.padding.w)
            }

            y += forcedH orelse me.rowSizes[j]
            y += me.spacingVert
        }

        if me.gridLayoutFlags.has(.AdaptSizeToContent):
            me.resize(maxW, maxH)
        me.wnd.IWnd.onResizeEvent(evt)
    }
}

impl GridLayoutCtrl
{
    // Create a layout control
    func create(parent: *Wnd, numColumns, numRows: s32, position: Math.Rectangle = {})->*GridLayoutCtrl
    {
        Debug.assert(numColumns > 0 and numRows > 0)

        let res = Wnd.create'GridLayoutCtrl(parent, position)
        res.numColumns = numColumns
        res.numRows    = numRows

        res.layout.resize(cast(u64) (numColumns * numRows))
        for numColumns:
            res.colSizes.add(100)
        for numRows:
            res.rowSizes.add(30)

        return res
    }

    // Recompute layout of all childs
    mtd computeLayout()
    {
        me.sendResizeEvent()
    }

    // Set size, in pixel, of a given column
    mtd setColSize(col: s32, size: f32)
    {
        me.colSizes[col] = size
    }

    // Set size, in pixel, of a given row
    mtd setRowSize(row: s32, size: f32)
    {
        me.rowSizes[row] = size
    }

    // Set the wnd associated with the given 'col' and 'row'.
    // Window can cover multiple columns and rows.
    mtd setColRowChild(child: *Wnd, col, row: s32, spanH, spanV: s32 = 1)
    {
        Debug.assert(child.parent == me)
        Debug.assert(col + spanH <= me.numColumns and row + spanV <= me.numRows)
        Debug.assert(spanH and spanV)

        for i in spanH
        {
            var x = col + (row * me.numColumns)
            x += i
            for j in spanV
            {
                let x1 = x + (j * me.numColumns)
                me.layout[x1] = child
            }
        }
    }
}
