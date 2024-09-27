#global public
using Core, Pixel

struct EditCtrl
{
    using frameWnd:     FrameWnd
    labelSize:          f32 = 60
    editSize:           f32 = 60

    label:              *Label
    edit:               *EditBox
}

impl IWnd for EditCtrl
{
    mtd impl onResizeEvent(evt: *ResizeEvent)
    {
        if !label:
            return

        var rect    = getClientRect()
        let metrics = getThemeMetrics()

        label.setPosition(0, 0, labelSize, metrics.editBox_Height)
        edit.setPosition(label.position.right() + 5, rect.vertCenter() - metrics.editBox_Height / 2, editSize, metrics.editBox_Height)
    }
}

impl EditCtrl
{
    // Create the popup list, but: not display it
    func create(parent: nn *Wnd, name: string, text: string, position: Math.Rectangle = {}, id: WndId = null, flags: EditBoxFlags = Zero)->*EditCtrl
    {
        let metrics = parent.getThemeMetrics()
        var pos     = position
        if pos.width == 0:
            pos.width = metrics.editBox_Width * 2
        if pos.height == 0:
            pos.height = metrics.editBox_Height

        with let ctrl = Wnd.create'EditCtrl(parent, pos, id)
        {
            .label                   = Label.create(ctrl, #self.name, {})
            .label.horzAlignmentText = .Right

            .edit              = EditBox.create(ctrl, text, {})
            .edit.editBoxFlags = flags

            .sendResizeEvent()
        }

        return ctrl
    }
}
