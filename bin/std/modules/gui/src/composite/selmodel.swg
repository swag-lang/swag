#global public
using Core

struct SelModel
{
    selection: Array'{start: u32, end: u32}
}

private impl SelModel
{
    mtd isSelectedRange(idx: u32)->u32
    {
        visit *r, i: selection
        {
            if idx >= r.start and idx <= r.end
                return cast(u32) i
        }

        return Swag.U32.Max
    }
}

impl SelModel
{
    mtd isEmpty()            => selection.isEmpty()
    mtd isSelected(idx: u32) => isSelectedRange(idx) != Swag.U32.Max

    mtd unselectAll()
    {
        selection.clear()
    }

    mtd select(idx: u32, select: bool)
    {
        isSelectedRange := isSelectedRange(idx)
        isSelected := isSelectedRange != Swag.U32.Max
        if select == isSelected
            return

        if select
        {
            // Prepend/Append to an existing range
            visit *r, i: selection
            {
                if r.start and idx == r.start - 1
                {
                    r.start -= 1
                    return
                }

                if idx == r.end + 1
                {
                    r.end += 1
                    return
                }
            }

            // Need to make a new range
            selection.add(@{idx, idx})
        }
        else
        {
            range := selection[isSelectedRange]
            if range.start == range.end
                selection.removeAt(cast(uint) isSelectedRange)
            elif range.start == idx
                range.start += 1
            elif range.end == idx
                range.end -= 1
            else
            {
                end := range.end
                range.end = idx - 1
                selection.add(@{idx + 1, end})
            }
        }
    }
}
