#global public
using Core, Pixel

struct ThemeStyleRef
{
    theme:     *Theme
    count:     u32
}

struct ThemeStyle
{
    refr:                  *ThemeStyleRef
    dirty:                 bool
    styleSheetMetrics:     String
    styleSheetColors:      String
    font:                  FontFamily
}

impl ThemeStyle
{
    mtd opDrop()
    {
        me.free()
    }

    internal mtd free()
    {
        if !me.refr:
            return
        me.refr.count -= 1
        if me.refr.count == 0
        {
            Memory.delete(me.refr.theme)
            Memory.delete(me.refr)
            me.refr = null
        }
    }

    internal mtd set(theme: *Theme)
    {
        me.refr       = Memory.new'ThemeStyleRef()
        me.refr.theme = theme
        me.refr.count = 1
    }

    // Create a specific style based on the parent
    mtd create(parent: *ThemeStyle)
    {
        me.free()
        var theme = Memory.new'Theme()
        dref theme = dref parent.refr.theme
        me.set(theme)
    }

    internal mtd compute(owner: *Wnd, parent: *ThemeStyle)
    {
        // First time, initialize to parent
        if !me.refr
        {
            me.refr = parent.refr
            me.refr.count += 1
            me.dirty = true
        }

        // If parent is dirty, or i am dirty, then we need
        // to compute.
        if !me.dirty and !parent.dirty:
            return
        me.dirty = true

        // By default, same as the parent
        me.free()
        me.refr = parent.refr
        me.refr.count += 1

        // Ask user
        var evt = ThemeEvent{}
        evt.accepted = false
        evt.style    = me
        evt.parent   = parent
        owner.sendEvent(&evt)

        // Nothing to override
        if me.styleSheetColors.length == 0 and
           me.styleSheetMetrics.length == 0 and
           me.font.regular == null and
           me.font.bold == null and
           me.font.italic == null and
           me.font.boldItalic == null and
           !evt.accepted
        {
            return
        }

        // Make a new tweak as a copy of the parent
        if me.refr == parent.refr:
            me.create(parent)

        // Then tweak it with the stylesheet
        if me.styleSheetColors.length
        {
            var tf: File.TweakFile
            tf.registerFolder'ThemeColors(&me.refr.theme.colors)
            catch tf.parse(me.styleSheetColors)
        }

        if me.styleSheetMetrics.length
        {
            var tf: File.TweakFile
            tf.registerFolder'ThemeMetrics(&me.refr.theme.metrics)
            catch tf.parse(me.styleSheetMetrics)
        }
    }

    mtd clearStyleSheetColors()
    {
        me.styleSheetColors.clear()
        me.dirty = true
    }

    mtd setStyleSheetColors(str: string)
    {
        me.styleSheetColors = str
        me.dirty            = true
    }

    mtd addStyleSheetColors(str: string)
    {
        me.styleSheetColors.append("\n")
        me.styleSheetColors.append(str)
        me.dirty = true
    }

    mtd setStyleSheetMetrics(str: string)
    {
        me.styleSheetMetrics = str
        me.dirty             = true
    }

    mtd addStyleSheetMetrics(str: string)
    {
        me.styleSheetMetrics.append("\n")
        me.styleSheetMetrics.append(str)
        me.dirty = true
    }

    mtd getFont()->FontFamily
    {
        var result: retval
        let df      = &me.refr.theme.res.fontDefault
        result.regular    = me.font.regular orelse df.regular
        result.bold       = me.font.bold orelse df.bold
        result.italic     = me.font.italic orelse df.italic
        result.boldItalic = me.font.boldItalic orelse df.boldItalic
        return result
    }

    #[Swag.Overload]
    mtd setFont(fnt: FontFamily)
    {
        me.font  = fnt
        me.dirty = true
    }

    #[Swag.Overload]
    mtd setFont(fnt: *Font, style: FontFamilyStyle)
    {
        me.font.setFont(fnt, style)
        me.dirty = true
    }

    mtd clearFont()
    {
        @init(&me.font, 1)
        me.dirty = true
    }
}
