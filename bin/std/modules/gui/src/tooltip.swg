#global namespace ToolTip

private
{
    struct ToolTipVal
    {
        owner:          *Wnd
        surfacePos:     Math.Point
        str:            String
        delay:          f32
        startDelay:     f32
    }

    var g_Pending:      *ToolTipVal
    var g_Current:      *FrameWnd
    var g_CurrentOwner: *Wnd
}

#drop
{
    hide()
}

func update(dt: f32)
{
    if !g_Pending do
        return

    g_Pending.delay -= dt
    if g_Pending.delay < 0
    {
        // Make a copy because g_Pending will be destroy by 'show'
        let copy = dref g_Pending
        show(copy.owner, copy.surfacePos, copy.str, 0's)
    }
}

// Hide the current tooltip, if visible
#[Swag.Overload]
public func hide()
{
    if g_Current do
        g_Current.destroy()

    g_Current      = null
    g_CurrentOwner = null

    Memory.delete(g_Pending)
    g_Pending = null
}

// Hide the current tooltip, if visible
#[Swag.Overload]
func hide(owner: *Wnd)
{
    if g_Pending and g_Pending.owner == owner do
        hide()
    elif owner == g_CurrentOwner do
        hide()
}

// Reset timer
func resetTimer(mousePos: Math.Point)
{
    if g_Pending
    {
        g_Pending.surfacePos = mousePos
        g_Pending.delay      = g_Pending.startDelay
    }
}

// Show a tooltip
public func show(owner: *Wnd, surfacePos: Math.Point, str: string, delay: Time.Duration = 500'ms)
{
    if str == null or @countof(str) == 0 do
        return

    if !g_Current and delay.timeInSeconds > 0
    {
        if !g_Pending do
            g_Pending = Memory.new'ToolTipVal()
        g_Pending.owner      = owner
        g_Pending.surfacePos = surfacePos
        g_Pending.str        = str
        g_Pending.startDelay = delay.timeInSeconds
        g_Pending.delay      = delay.timeInSeconds
        return
    }

    let copy: String = str
    hide()

    let top = owner.surface.wnd
    var pos = top.surfaceToLocal(surfacePos)
    pos.offset(12)

    // Frame container
    g_Current = Wnd.create'FrameWnd(top, {pos.x, pos.y, 50, 100})
    g_Current.setFrameForm(.Square)
    g_Current.wndFlags.add(.TopMost)
    g_Current.style.addStyleSheetColors("frameWnd_Bk     $toolTip_Bk")
    g_Current.style.addStyleSheetColors("frameWnd_Border $toolTip_Border")
    g_Current.padding = 0
    g_CurrentOwner    = owner

    // View
    let view = g_Current.createView'Wnd()
    view.backgroundStyle = .Transparent
    view.margin          = 0

    let colors  = top.getThemeColors()
    let metrics = top.getThemeMetrics()

    // Rich edit
    let edit = RichEditCtrl.create(view)
    edit.setRichEdFlags(.WordWrap)
    edit.setForm(.Edit)
    edit.styles[0].colFg = colors.toolTip_Text
    g_Current.sendResizeEvent()
    edit.setText(copy)
    edit.margin    = metrics.toolTip_Padding
    edit.dockStyle = .Center

    // Auto size
    var (w, h) = edit.evaluateSize()
    w += 2 * metrics.toolTip_Padding
    h += 2 * metrics.toolTip_Padding
    g_Current.setPosition(Math.Rectangle{pos.x, pos.y, w, h})
    g_Current.fitPosInParent()
    g_Current.bringToTop()
}
