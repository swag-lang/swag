#global public
using Core

struct StyleRef
{
    palette:    *Palette
    count:      u32
}

struct Style
{
    ref:            *StyleRef
    dirtyCount:     u32
    styleSheet:     String
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
    protected mtd set(palette: *Palette)
    {
        if ref
            ref.release()
        ref = Memory.new'StyleRef()
        ref.palette = palette
        ref.count = 1
    }

    protected mtd compute(parent: *Style)
    {
        if !ref
        {
            ref = parent.ref
            ref.count += 1
        }

        if parent.dirtyCount == dirtyCount
            return
        dirtyCount = parent.dirtyCount

        // Same as parent, and nothing to override, exit
        if ref == parent.ref and styleSheet.length == 0
            return

        palette := Memory.new'Palette()
        set(palette)

        var tf: File.TweakFile
        tf.registerFolder'Palette(palette)
        catch tf.parse(styleSheet)
        if @err
            @print(@err)
    }
}