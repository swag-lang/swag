using Core, Gui, Pixel

func createView2(view: *Wnd)
{
    {
        split := SplitView.create(view, @{10, 10, 300, 300}, vertical: true)
        split.setFrameForm(FrameWndForm.Round)

        list0 := createListSimple(split)
        list0.setFrameForm(FrameWndForm.Transparent)
        list1 := createListSimple(split)
        list1.setFrameForm(FrameWndForm.Transparent)
        split.sendResizeEvent()
    }

    {
        split := SplitView.create(view, @{320, 10, 400, 300}, vertical: false)
        split.frameFlags |= .FocusBorder
        split.setFrameForm(FrameWndForm.Square)

        list0 := createListSimple(split)
        list0.setFrameForm(FrameWndForm.Transparent)
        list1 := createListMulti(split)
        list1.setFrameForm(FrameWndForm.Transparent)
        list1.setAllMargins(5)
        split.sendResizeEvent()
    }
}