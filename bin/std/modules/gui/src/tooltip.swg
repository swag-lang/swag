#global namespace ToolTip

using namespace ToolTipPriv {

var g_Pending:      bool
var g_PendingTime:  f32
var g_Current:      *FrameWnd

}

func update(dt: f32)
{
    if !g_Pending
        return
    g_PendingTime += dt
}

public func hide()
{
    if g_Current
        g_Current.destroy()
    g_Current = null
}

public func show(surface: *Surface, surfacePos: Math.Point, str: string)
{
    hide()

    pos := surfacePos

    g_Current = Wnd.create'FrameWnd(surface.wnd, @{pos.x, pos.y, 50, 100})
    g_Current.setFrameForm(.Square)
    g_Current.setWndFlags(.TopMost)
    g_Current.style.addStyleSheetColors("frameWnd_Bk     $toolTip_Bk")
    g_Current.style.addStyleSheetColors("frameWnd_Border 0xFF000000")
    g_Current.padding = 0

    view := g_Current.createView'Wnd()
    view.backgroundStyle = .Transparent
    view.margin = 0

    colors  := surface.wnd.getThemeColors()
    metrics := surface.wnd.getThemeMetrics()

    edit := RichEditCtrl.create(view)
    edit.setRichEdFlags(.WordWrap)
    edit.setForm(.Edit)
    edit.styles[0].colFg = colors.toolTip_Text
    g_Current.sendResizeEvent()
    edit.setText(str)
    edit.margin = metrics.toolTip_Padding
    edit.dockStyle = .Center

    (w, h) := edit.evaluateSize()
    w += 2 * metrics.toolTip_Padding
    h += 2 * metrics.toolTip_Padding
    g_Current.setPosition(Math.Rectangle{pos.x, pos.y, w, h})
    g_Current.bringToTop()
}