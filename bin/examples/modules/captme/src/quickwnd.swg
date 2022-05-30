using Core, Gui, Pixel

struct QuickWnd
{
    using wnd:  Wnd
    main:       *MainWnd
    styles:     *QuickStyle
}

impl IWnd for QuickWnd
{
    mtd onPaintEvent(evt: *PaintEvent)
    {
    }
}

impl QuickWnd
{
    mtd loadQuickStyles()
    {
        qs := Memory.new'QuickStyle()
        styles = qs

        formc := Memory.new'FormArrow()
        formc.xa = 8
        formc.ya = 40
        formc.xb = 40
        formc.yb = 8
        formc.borderSize = 5
        formc.endSize = 10
        formc.endShape = .Arrow
        formc.borderColor = Argb.Red
        it := cast(IForm) formc
        it.setup(null)
        styles.forms.add(it)
    }

    mtd updateQuickStyles()
    {
        while childs.count
            childs[0].destroyNow()
        if !styles
            return

        loop i: styles.forms
        {
            btn := IconButton.create(self, @{}, @{0, 0, 48, 48})
            f := styles.forms[i]
            btn.sigPaintIcon = @|f|(btn, bc, pos) {
                bc.painter.setParams(.Antialiased)
                f.paint(bc.painter, 1)
            }
        }
    }
}