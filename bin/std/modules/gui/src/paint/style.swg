#global public
using Core

struct StyleRef
{
    palette:    *Palette
    count:      u32
}

struct Style
{
    ref: *StyleRef
}

impl StyleRef
{
    mtd release()
    {
        if count == 0
            return
        count -= 1
        if count == 0
        {
            Memory.delete(palette)
            palette = null
        }
    }
}

impl Style
{
    mtd compute(parent: *Style)
    {
        if !ref
        {
            ref = parent.ref
            ref.count += 1
        }
    }
}