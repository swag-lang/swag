#global public
using Core, Pixel

struct ComboCtrl
{
    using frameWnd:     FrameWnd
    labelSize:          f32 = 60
    comboSize:          f32 = 60

    label:              *Label
    combo:              *ComboBox
}

impl IWnd for ComboCtrl
{
    mtd impl onResizeEvent(evt: *ResizeEvent)
    {
        if !.label do
            return

        let rect    = .getClientRect()
        let metrics = .getThemeMetrics()

        .label.setPosition(0, 0, .labelSize, metrics.comboBox_Height)
        .combo.setPosition(.label.position.right() + 5, rect.vertCenter() - metrics.comboBox_Height / 2, .comboSize, metrics.comboBox_Height)
    }
}

impl ComboCtrl
{
    func create(parent: *Wnd, name: string, position: Math.Rectangle = {}, id: WndId = null)->*ComboCtrl
    {
        let metrics = parent.getThemeMetrics()
        var pos     = position
        if pos.width == 0 do
            pos.width = metrics.comboBox_Width * 2
        if pos.height == 0 do
            pos.height = metrics.comboBox_Height

        with let ctrl = Wnd.create'ComboCtrl(parent, pos, id)
        {
            .label                   = Label.create(ctrl, name)
            .label.horzAlignmentText = .Right

            .combo = ComboBox.create(ctrl)

            .sendResizeEvent()
        }

        return ctrl
    }
}
