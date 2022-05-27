using Core, Gui, Pixel

struct QuickWnd
{
    using wnd:  Wnd
    main:       *MainWnd
}

impl IWnd for QuickWnd
{
    mtd onPaintEvent(evt: *PaintEvent)
    {
        /*formc := Memory.new'FormArrow()
        formc.xa = 0
        formc.ya = 40
        formc.xb = 40
        formc.yb = 0
        formc.borderSize = 5
        formc.endSize = 10
        formc.endShape = .Arrow
        it := cast(IForm) formc
        it.setup(null)

        it.paint(evt.bc.painter, 1)*/
    }
}