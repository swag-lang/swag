#global public
using Core

struct SelModel
{
    selection: Array'{ start: u32, end: u32 }
}

impl SelModel
{
    mtd getSelectedRangeIndex(idx: u32)->u32
    {
        foreach &r, i in selection
        {
            if idx >= r.start and idx <= r.end:
                return cast(u32) i
        }

        return Swag.U32.Max
    }

    #[Swag.Inline]
    {
        mtd isEmpty()            => selection.isEmpty()
        mtd isSelected(idx: u32) => getSelectedRangeIndex(idx) != Swag.U32.Max
    }

    // Unselect all
    mtd unselectAll()
    {
        selection.clear()
    }

    // Select a given range
    #[Swag.Overload]
    mtd select(start, end: u32, select: bool)
    {
        if start <= end
        {
            for i in start to end:
                select(i, select)
        }
        else
        {
            for i in end to start:
                select(i, select)
        }
    }

    // Select the given index
    #[Swag.Overload]
    mtd select(idx: u32, select: bool)
    {
        let isSelectedRange = getSelectedRangeIndex(idx)
        let isSelected      = isSelectedRange != Swag.U32.Max
        if select == isSelected:
            return

        if select
        {
            // Prepend/Append to an existing range
            foreach &r, i in selection
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
            selection.add({idx, idx})
        }
        else
        {
            let range = #ref selection[isSelectedRange]
            if range.start == range.end:
                selection.removeAt(cast(u64) isSelectedRange)
            elif range.start == idx:
                range.start += 1
            elif range.end == idx:
                range.end -= 1
            else
            {
                let end = range.end
                range.end = idx - 1
                selection.add({idx + 1, end})
            }
        }
    }

    #[Swag.Macro]
    mtd(ptr: bool, back: bool) opVisit(stmt: code void)
    {
        #scope Up
        foreach &r in selection
        {
            for i in r.start to r.end
            {
                #macro
                {
                    let #alias0 = #up i
                    let #alias1 = #index
                    #inject #up stmt => { break = break to Up; }
                }
            }
        }
    }
}
