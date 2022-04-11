#global public
using Core

struct ThemeStyleRef
{
    theme:  *Theme
    count:  u32
}

struct ThemeStyle
{
    ref:                *ThemeStyleRef
    dirty:              bool
    styleSheetMetrics:  String
    styleSheetColors:   String
}

impl ThemeStyle
{
    private mtd opDrop()
    {
        release()
    }

    private mtd release()
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

    private mtd set(theme: *Theme)
    {
        ref = Memory.new'ThemeStyleRef()
        ref.theme = theme
        ref.count = 1
    }

    // Create a specific style based on the parent
    mtd create(parent: *ThemeStyle)
    {
        release()
        theme := Memory.new'Theme()
        dref theme = dref parent.ref.theme
        set(theme)
    }

    private mtd compute(owner: *Wnd, parent: *ThemeStyle)
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

    mtd clearStyleSheetColors()
    {
        styleSheetColors.clear()
        dirty = true
    }

    mtd setStyleSheetColors(str: string)
    {
        styleSheetColors = str
        dirty = true
    }

    mtd addStyleSheetColors(str: string)
    {
        styleSheetColors.append("\n")
        styleSheetColors.append(str)
        dirty = true
    }

    mtd setStyleSheetMetrics(str: string)
    {
        styleSheetMetrics = str
        dirty = true
    }

    mtd addStyleSheetMetrics(str: string)
    {
        styleSheetMetrics.append("\n")
        styleSheetMetrics.append(str)
        dirty = true
    }
}