#global public
using Core

struct SelModel
{
    selection: Array'{ start: u32, end: u32 }
}

impl SelModel
{
    mtd const getSelectedRangeIndex(idx: u32)->u32
    {
        foreach &r, i in .selection
        {
            if idx >= r.start and idx <= r.end do
                return cast(u32) i
        }

        return Swag.U32.Max
    }

    #[Swag.Inline]
    {
        mtd const isEmpty()            => .selection.isEmpty()
        mtd const isSelected(idx: u32) => .getSelectedRangeIndex(idx) != Swag.U32.Max
    }

    // Unselect all
    mtd unselectAll()
    {
        .selection.clear()
    }

    // Select a given range
    #[Swag.Overload]
    mtd select(start, end: u32, select: bool)
    {
        if start <= end
        {
            for i in start to end do
                .select(i, select)
        }
        else
        {
            for i in end to start do
                .select(i, select)
        }
    }

    // Select the given index
    #[Swag.Overload]
    mtd select(idx: u32, select: bool)
    {
        let isSelectedRange = .getSelectedRangeIndex(idx)
        let isSelected      = isSelectedRange != Swag.U32.Max
        if select == isSelected do
            return

        if select
        {
            // Prepend/Append to an existing range
            foreach &r, i in .selection
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
            .selection.add({idx, idx})
        }
        else
        {
            let range = #ref .selection[isSelectedRange]
            if range.start == range.end do
                .selection.removeAt(cast(u64) isSelectedRange)
            elif range.start == idx do
                range.start += 1
            elif range.end == idx do
                range.end -= 1
            else
            {
                let end = range.end
                range.end = idx - 1
                .selection.add({idx + 1, end})
            }
        }
    }

    #[Swag.Macro]
    mtd(ptr: bool, back: bool) const opVisit(stmt: #code void)
    {
        #scope(Up)
        foreach &r in .selection
        {
            for i in r.start to r.end
            {
                #macro
                {
                    let #alias0 = #up i
                    let #alias1 = @index
                    #inject(#up stmt, break = break to Up)
                }
            }
        }
    }
}
