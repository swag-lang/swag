#global namespace ToolTip

using namespace ToolTipPriv {

struct ToolTip
{
    owner:      *Wnd
    surfacePos: Math.Point
    str:        String
    delay:      f32
}

var g_Pending:      *ToolTip
var g_Current:      *FrameWnd
var g_CurrentOwner: *Wnd

}

func update(dt: f32)
{
    if !g_Pending
        return

    g_Pending.delay -= dt
    if g_Pending.delay < 0
    {
        show(g_Pending.owner, g_Pending.surfacePos, g_Pending.str, 0's)
    }
}

// Hide the current tooltip, if visible
public func hide()
{
    if g_Current
        g_Current.destroy()
    g_Current = null
    g_CurrentOwner = null

    Memory.delete(g_Pending)
    g_Pending = null
}

// Hide the current tooltip, if visible
public func hide(owner: *Wnd)
{
    if g_Pending and g_Pending.owner == owner
        hide()
    elif owner == g_CurrentOwner
        hide()
}

// Show a tooltip
public func show(owner: *Wnd, surfacePos: Math.Point, str: string, delay: Time.Duration = 500'ms)
{
    if !g_Current and delay.timeInSeconds > 0
    {
        if !g_Pending
            g_Pending = Memory.new'ToolTip()
        g_Pending.owner = owner
        g_Pending.surfacePos = surfacePos
        g_Pending.str = str
        g_Pending.delay = delay.timeInSeconds
        return
    }

    var copy: String = str
    hide()

    top := owner.surface.wnd
    pos := top.surfaceToLocal(surfacePos)

    // Frame container
    g_Current = Wnd.create'FrameWnd(top, @{pos.x, pos.y, 50, 100})
    g_Current.setFrameForm(.Square)
    g_Current.setWndFlags(.TopMost)
    g_Current.style.addStyleSheetColors("frameWnd_Bk     $toolTip_Bk")
    g_Current.style.addStyleSheetColors("frameWnd_Border 0xFF000000")
    g_Current.padding = 0
    g_CurrentOwner = owner

    // View
    view := g_Current.createView'Wnd()
    view.backgroundStyle = .Transparent
    view.margin = 0

    colors  := top.getThemeColors()
    metrics := top.getThemeMetrics()

    // Rich edit
    edit := RichEditCtrl.create(view)
    edit.setRichEdFlags(.WordWrap)
    edit.setForm(.Edit)
    edit.styles[0].colFg = colors.toolTip_Text
    g_Current.sendResizeEvent()
    edit.setText(copy)
    edit.margin = metrics.toolTip_Padding
    edit.dockStyle = .Center

    // Auto size
    (w, h) := edit.evaluateSize()
    w += 2 * metrics.toolTip_Padding
    h += 2 * metrics.toolTip_Padding
    g_Current.setPosition(Math.Rectangle{pos.x, pos.y, w, h})
    g_Current.bringToTop()
}