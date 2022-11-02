using Pixel

#[Swag.Opaque]
public struct PaintToImage
{
    image:  Image
    w, h:   s32
    paint:  closure(app: *Application, painter: *Painter)
}

impl PaintToImage
{
    private func onEvent(wnd: *Wnd, evt: *Event)->bool
    {
        if evt.kind != .Paint
            return false

        var painter: Painter
        pt := cast(*PaintToImage) wnd.surface.userData
        painter.begin()
        painter.setParams(.Antialiased)
        painter.clear(Argb.Zero)
        pt.paint(wnd.getApp(), &painter)
        painter.end()

        r := &wnd.getApp().renderer
        rt := r.getRenderTarget(pt.w, pt.h)
        r.bindRenderTarget(rt)
        r.draw(&painter)
        pt.image = r.unbindRenderTargetToImage(0, 0, pt.w, pt.h)
        r.end()
        r.releaseRenderTarget(rt)

        wnd.postQuitEvent()
        return false
    }

    public func draw(w, h: s32, call: closure(*Application, *Painter))->Image
    {
        var pt: PaintToImage
        var app: Application
        surface := assume app.createSurface(0, 0, 1, 1, hook: &onEvent)
        surface.userData = &pt
        pt.w = #self.w
        pt.h = #self.h
        pt.paint = call
        app.runFrame()
        surface.destroy()
        return pt.image
    }
}
