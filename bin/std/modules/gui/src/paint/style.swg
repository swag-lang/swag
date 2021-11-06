#global public
using Core

struct StyleRef
{
    theme:  *Theme
    count:  u32
}

struct Style
{
    ref:                *StyleRef
    dirty:              bool
    styleSheetMetrics:  String
    styleSheetColors:   String
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
            Memory.delete(ref.theme)
            Memory.delete(ref)
            ref = null
        }
    }

    protected mtd set(theme: *Theme)
    {
        ref = Memory.new'StyleRef()
        ref.theme = theme
        ref.count = 1
    }

    // Create a specific style based on the parent
    mtd create(parent: *Style)
    {
        release()
        theme := Memory.new'Theme()
        dref theme = dref parent.ref.theme
        set(theme)
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

        // If parent is dirty, or i am dirty, then we need
        // to compute.
        if !dirty and !parent.dirty
            return
        dirty = true

        // By default, same as the parent
        release()
        ref = parent.ref
        ref.count += 1

        // Ask user
        evt := SetThemeEvent{}
        evt.accepted = false
        evt.style = self
        evt.parent = parent
        owner.sendEvent(&evt)

        // Nothing to override
        if styleSheetColors.length == 0 and styleSheetMetrics.length == 0 and !evt.accepted
            return

        // Make a new tweak as a copy of the parent
        if ref == parent.ref
            create(parent)

        // Then tweak it with the stylesheet
        if styleSheetColors.length
        {
            var tf: File.TweakFile
            tf.registerFolder'ThemeColors(&ref.theme.colors)
            catch tf.parse(styleSheetColors)
        }

        if styleSheetMetrics.length
        {
            var tf: File.TweakFile
            tf.registerFolder'ThemeMetrics(&ref.theme.metrics)
            catch tf.parse(styleSheetMetrics)
        }
    }

    mtd setStyleSheetColors(str: String)
    {
        styleSheetColors = str
        dirty = true
    }

    mtd setStyleSheetMetrics(str: String)
    {
        styleSheetMetrics = str
        dirty = true
    }
}