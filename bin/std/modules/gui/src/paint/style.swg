#global public
using Core

struct StyleRef
{
    tweak:    *Tweak
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
            Memory.delete(ref.tweak)
            Memory.delete(ref)
            ref = null
        }
    }

    protected mtd set(tweak: *Tweak)
    {
        ref = Memory.new'StyleRef()
        ref.tweak = tweak
        ref.count = 1
    }

    protected mtd compute(owner: *Wnd, parent: *Style)
    {
        // First time, initialize to parent
        if !ref
        {
            ref = parent.ref
            ref.count += 1
            dirty = true
        }

        // Ask user
        evt := SysCommandEvent{}
        evt.sysKind = .SetTheme
        owner.sendEvent(&evt)

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

        // Make a new tweak as a copy of the parent
        tweak := Memory.new'Tweak()
        dref tweak = dref parent.ref.tweak
        set(tweak)

        // Then tweak it with the stylesheet
        var tf: File.TweakFile
        tf.registerFolder'Tweak(tweak)
        catch tf.parse(styleSheet)
    }
}