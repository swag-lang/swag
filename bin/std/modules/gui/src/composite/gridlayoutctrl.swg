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
        if !.childs.count
        {
            .wnd.IWnd.onResizeEvent(evt)
            return
        }

        // Auto compute width of each cell
        var forcedW = 0'f32
        if .gridLayoutFlags.has(.AdaptColWidth)
        {
            forcedW = .position.width
            forcedW -= (.padding.x + .padding.z)
            forcedW -= (.numColumns - 1) * .spacingHorz
            forcedW /= .numColumns
        }

        // Auto compute height of each cell
        var forcedH = 0'f32
        if .gridLayoutFlags.has(.AdaptRowHeight)
        {
            forcedH = .position.height
            forcedH -= (.padding.y + .padding.w)
            forcedH -= (.numRows - 1) * .spacingVert
            forcedH /= .numRows
        }

        var done: Array'(*Wnd)
        var maxW, maxH = 0'f32
        var y = .padding.y
        for j in .numRows
        {
            var x = .padding.x
            for i in .numColumns
            {
                let colW = forcedW orelse .colSizes[i]
                defer x += colW + .spacingHorz

                let idx    = i + (j * .numColumns)
                let curWnd = .layout[idx]
                if !curWnd or done.contains(curWnd) do
                    continue

                done.add(curWnd)

                var w = 0'f32
                var h = 0'f32

                var idxC = i
                var idxW = idx
                var idxN = 0
                while idxC < .numColumns and .layout[idxW] == curWnd
                {
                    w += forcedW orelse .colSizes[idxC]
                    idxW, idxC, idxN += 1
                }

                w += (idxN - 1) * .spacingHorz

                idxC = j
                idxW = idx
                idxN = 0
                while idxC < .numRows and .layout[idxW] == curWnd
                {
                    h += forcedH orelse .rowSizes[idxC]
                    idxW += .numColumns
                    idxC, idxN += 1
                }

                h += (idxN - 1) * .spacingVert

                curWnd.setPosition(Math.Rectangle{x, y, w, h})
                maxW = Math.max(maxW, x + w + .padding.z)
                maxH = Math.max(maxH, y + h + .padding.w)
            }

            y += forcedH orelse .rowSizes[j]
            y += .spacingVert
        }

        if .gridLayoutFlags.has(.AdaptSizeToContent) do
            .resize(maxW, maxH)
        .wnd.IWnd.onResizeEvent(evt)
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
        for numColumns do
            res.colSizes.add(100)
        for numRows do
            res.rowSizes.add(30)

        return res
    }

    // Recompute layout of all childs
    mtd computeLayout()
    {
        .sendResizeEvent()
    }

    // Set size, in pixel, of a given column
    mtd setColSize(col: s32, size: f32)
    {
        .colSizes[col] = size
    }

    // Set size, in pixel, of a given row
    mtd setRowSize(row: s32, size: f32)
    {
        .rowSizes[row] = size
    }

    // Set the wnd associated with the given 'col' and 'row'.
    // Window can cover multiple columns and rows.
    mtd setColRowChild(child: *Wnd, col, row: s32, spanH, spanV: s32 = 1)
    {
        Debug.assert(child.parent == me)
        Debug.assert(col + spanH <= .numColumns and row + spanV <= .numRows)
        Debug.assert(spanH and spanV)

        for i in spanH
        {
            var x = col + (row * .numColumns)
            x += i
            for j in spanV
            {
                let x1 = x + (j * .numColumns)
                .layout[x1] = child
            }
        }
    }
}
