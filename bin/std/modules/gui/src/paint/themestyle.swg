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
        .free()
    }

    internal mtd free()
    {
        if !.refr do
            return
        .refr.count -= 1
        if .refr.count == 0
        {
            Memory.delete(.refr.theme)
            Memory.delete(.refr)
            .refr = null
        }
    }

    internal mtd set(theme: *Theme)
    {
        .refr       = Memory.new'ThemeStyleRef()
        .refr.theme = theme
        .refr.count = 1
    }

    // Create a specific style based on the parent
    mtd create(parent: *ThemeStyle)
    {
        .free()
        var theme = Memory.new'Theme()
        dref theme = dref parent.refr.theme
        .set(theme)
    }

    internal mtd compute(owner: *Wnd, parent: *ThemeStyle)
    {
        // First time, initialize to parent
        if !.refr
        {
            .refr = parent.refr
            .refr.count += 1
            .dirty = true
        }

        // If parent is dirty, or i am dirty, then we need
        // to compute.
        if !.dirty and !parent.dirty do
            return
        .dirty = true

        // By default, same as the parent
        .free()
        .refr = parent.refr
        .refr.count += 1

        // Ask user
        var evt = ThemeEvent{}
        evt.accepted = false
        evt.style    = me
        evt.parent   = parent
        owner.sendEvent(&evt)

        // Nothing to override
        if .styleSheetColors.length == 0 and
           .styleSheetMetrics.length == 0 and
           .font.regular == null and
           .font.bold == null and
           .font.italic == null and
           .font.boldItalic == null and
           !evt.accepted
        {
            return
        }

        // Make a new tweak as a copy of the parent
        if .refr == parent.refr do
            .create(parent)

        // Then tweak it with the stylesheet
        if .styleSheetColors.length
        {
            var tf: File.TweakFile
            tf.registerFolder'ThemeColors(&.refr.theme.colors)
            catch tf.parse(.styleSheetColors)
        }

        if .styleSheetMetrics.length
        {
            var tf: File.TweakFile
            tf.registerFolder'ThemeMetrics(&.refr.theme.metrics)
            catch tf.parse(.styleSheetMetrics)
        }
    }

    mtd clearStyleSheetColors()
    {
        .styleSheetColors.clear()
        .dirty = true
    }

    mtd setStyleSheetColors(str: string)
    {
        .styleSheetColors = str
        .dirty            = true
    }

    mtd addStyleSheetColors(str: string)
    {
        .styleSheetColors.append("\n")
        .styleSheetColors.append(str)
        .dirty = true
    }

    mtd setStyleSheetMetrics(str: string)
    {
        .styleSheetMetrics = str
        .dirty             = true
    }

    mtd addStyleSheetMetrics(str: string)
    {
        .styleSheetMetrics.append("\n")
        .styleSheetMetrics.append(str)
        .dirty = true
    }

    mtd const getFont()->FontFamily
    {
        var result: retval
        let df      = &.refr.theme.res.fontDefault
        result.regular    = .font.regular orelse df.regular
        result.bold       = .font.bold orelse df.bold
        result.italic     = .font.italic orelse df.italic
        result.boldItalic = .font.boldItalic orelse df.boldItalic
        return result
    }

    #[Swag.Overload]
    mtd setFont(fnt: FontFamily)
    {
        .font  = fnt
        .dirty = true
    }

    #[Swag.Overload]
    mtd setFont(fnt: *Font, style: FontFamilyStyle)
    {
        .font.setFont(fnt, style)
        .dirty = true
    }

    mtd clearFont()
    {
        @init(&.font, 1)
        .dirty = true
    }
}
