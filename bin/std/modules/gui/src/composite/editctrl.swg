#global public
using Core, Pixel

struct EditCtrl
{
    using frameWnd:     FrameWnd
    labelSize:          f32 = 60
    editSize:           f32 = 60     // 0 to fit to parent size

    label:              *Label
    edit:               *EditBox
}

impl IWnd for EditCtrl
{
    mtd impl onResizeEvent(evt: *ResizeEvent)
    {
        if !.label do
            return

        let rect    = .getClientRect()
        let metrics = .getThemeMetrics()

        let y = rect.vertCenter() - metrics.editBox_Height / 2
        .label.setPosition(0, y, .labelSize, metrics.editBox_Height)
        if .editSize == 0 do
            .edit.setPosition(.label.position.right() + 5, y, .position.width - 5 - .labelSize, metrics.editBox_Height)
        else do
            .edit.setPosition(.label.position.right() + 5, y, .editSize, metrics.editBox_Height)
    }
}

impl EditCtrl
{
    // Create the popup list, but: not display it
    func create(parent: *Wnd, name: string, text: string, position: Math.Rectangle = {}, id: WndId = null, flags: EditBoxFlags = Zero)->*EditCtrl
    {
        let metrics = parent.getThemeMetrics()
        var pos     = position
        if pos.width == 0 do
            pos.width = metrics.editBox_Width * 2
        if pos.height == 0 do
            pos.height = metrics.editBox_Height

        with let ctrl = Wnd.create'EditCtrl(parent, pos, id)
        {
            .label                   = Label.create(ctrl, name, {})
            .label.horzAlignmentText = .Right
            .edit                    = EditBox.create(ctrl, text, {})
            .edit.editBoxFlags       = flags
            .sendResizeEvent()
        }

        return ctrl
    }
}
