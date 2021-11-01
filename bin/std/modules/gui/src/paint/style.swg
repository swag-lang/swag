#global public
using Core

struct StyleRef
{
    palette:    *Palette
    count:      u32
}

struct Style
{
    ref:        *StyleRef
    dirty:      bool
    styleSheet: String
}

impl Style
{
    protected mtd opDrop()
    {
        release()
    }

    protected mtd release()
    {
        if !ref
            return
        ref.count -= 1
        if ref.count == 0
        {
            Memory.delete(ref.palette)
            Memory.delete(ref)
            ref = null
        }
    }

    protected mtd set(palette: *Palette)
    {
        ref = Memory.new'StyleRef()
        ref.palette = palette
        ref.count = 1
    }

    protected mtd compute(parent: *Style)
    {
        // First time, initialize to parent
        if !ref
        {
            ref = parent.ref
            ref.count += 1
            dirty = true
        }

        // If parent is dirty, or i am dirty, then we need
        // to compute.
        if !dirty and !parent.dirty
            return
        dirty = true

        // Nothing to override
        release()
        if styleSheet.length == 0
        {
            ref = parent.ref
            ref.count += 1
            return
        }

        // Make a new palette as a copy of the parent
        palette := Memory.new'Palette()
        dref palette = dref parent.ref.palette
        set(palette)

        // Then tweak it with the stylesheet
        var tf: File.TweakFile
        tf.registerFolder'Palette(palette)
        catch tf.parse(styleSheet)
    }
}