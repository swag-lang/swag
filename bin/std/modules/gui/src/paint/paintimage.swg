#global marked
using Pixel

#[Swag.Opaque]
public struct PaintImage
{
    app:         Application
    painter:     Painter
    surface:     *Surface
    image:       Image
    w, h:        s32
    paint:       func||(app: *Application, painter: *Painter)
}

impl PaintImage
{
    internal func onEvent(wnd: *Wnd, evt: IEvent)->bool
    {
        if !evt is PaintEvent:
            return false

        let pt = cast(*PaintImage) wnd.surface.userData
        if !pt.paint:
            return false

        let painter = &pt.painter
        painter.begin()
        painter.setParams(.Antialiased)
        painter.clear(Argb.Zero)
        pt.paint(wnd.getApp(), painter)
        painter.end()

        let r  = &wnd.getApp().renderer
        let rt = r.getRenderTarget(pt.w, pt.h)
        r.bindRenderTarget(rt)
        r.draw(painter)
        pt.image = r.unbindRenderTargetToImage(0, 0, pt.w, pt.h)
        r.end()
        r.releaseRenderTarget(rt)

        return false
    }

    public mtd opDrop()
    {
        if me.surface:
            me.surface.destroy()
    }

    // First init the PaintImage instance
    public mtd init(w, h: s32)
    {
        me.surface          = assume me.app.createSurface(0, 0, 1, 1, hook: &onEvent)
        me.surface.userData = me
        me.w                = #me.w
        me.h                = #me.h
    }

    // Draw to an image by calling the func
    #[Swag.Overload]
    public mtd draw(call: func||(*Application, *Painter))->Image
    {
        me.paint = call
        me.surface.invalidate()
        me.app.runFrame()
        return me.image
    }

    // One shot paint to image
    #[Swag.Overload]
    public func draw(w, h: s32, call: func||(*Application, *Painter))->Image
    {
        var pt: PaintImage
        pt.init(w, h)
        return pt.draw(call)
    }
}
