#global public
using Core, Pixel

struct SliderCtrl
{
    using frameWnd: FrameWnd
    labelSize:  f32 = 60
    editSize:   f32 = 60

    label:  *Label
    slider: *Slider
    edit:   *EditBox
}

impl IWnd for SliderCtrl
{
    mtd onResizeEvent(evt: *ResizeEvent)
    {
        if !label return

        rect    := getClientRectPadding()
        metrics := getThemeMetrics()

        label.setPosition(0, 0, labelSize, rect.height)

        w := rect.width - labelSize - editSize - 4
        slider.setPosition(label.position.right() + 4, rect.vertCenter() - metrics.slider_Height / 2, w, metrics.slider_Height)

        edit.setPosition(slider.position.right(), rect.vertCenter() - metrics.editBox_Height / 2, editSize, metrics.editBox_Height)
    }
}

impl SliderCtrl
{
    // Create the popup list, but do not display it
    func create(parent: *Wnd, name: string, position: Math.Rectangle, id: WndId = null)->*SliderCtrl
    {
        metrics := parent.getThemeMetrics()
        pos := position
        if pos.width == 0
            pos.width = metrics.slider_Width + metrics.editBox_Width * 2
        if pos.height == 0
            pos.height = Math.max(metrics.slider_Height, metrics.editBox_Height)

        ctrl := Wnd.create'SliderCtrl(parent, pos, id)

        with ctrl
        {
            label = Label.create(ctrl, #self.name, @{})
            label.horzAlignmentText = .Right

            slider = Slider.create(ctrl, @{})
            slider.sigChanged += @|ctrl|(wnd)
            {
                var val: String
                switch ctrl.edit.inputMode
                {
                case .U64: val = Format.toString("%", cast(u64) wnd.getValue())
                case .S64: val = Format.toString("%", cast(s64) wnd.getValue())
                case .F64: val = Format.toString("%", cast(f64) wnd.getValue())
                }

                ctrl.edit.setText(val)
            }

            edit = EditBox.create(ctrl, "", @{})
            edit.sigLoseFocus += @|ctrl|(wnd)
            {
                switch ctrl.edit.inputMode
                {
                case .U64:
                    (val, ?, ?) := catch StrConv.parseU64(wnd.text)
                    ctrl.slider.setValue(cast(f32) val)

                case .S64:
                    (val, ?, ?) := catch StrConv.parseS64(wnd.text)
                    ctrl.slider.setValue(cast(f32) val)

                case .F64:
                    (val, ?, ?) := catch StrConv.parseF64(wnd.text)
                    ctrl.slider.setValue(cast(f32) val)
                }
            }

            sendResizeEvent()
        }

        return ctrl
    }
}
