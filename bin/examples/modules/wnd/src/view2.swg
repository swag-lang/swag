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

        split.addPane(list0)
        split.addPane(list1)
    }

    {
        split := SplitView.create(view, @{320, 10, 400, 300}, vertical: false)
        split.frameFlags |= .FocusBorder
        split.setFrameForm(FrameWndForm.Square)

        list0 := createListSimple(split)
        list0.setFrameForm(FrameWndForm.Transparent)
        list1 := createListMulti(split)
        list1.setFrameForm(FrameWndForm.Transparent)

        split.addPane(list0)
        split.addPane(list1)
    }

    {
        split := SplitView.create(view, @{10, 320, 600, 300}, vertical: false)
        split.dockStyle = .Bottom
        split.setFrameForm(FrameWndForm.Square)

        list0 := createListSimple(split)
        list1 := createListMulti(split)
        list2 := createListSimple(split)
        list0.setFrameForm(FrameWndForm.Transparent)
        list1.setFrameForm(FrameWndForm.Transparent)
        list2.setFrameForm(FrameWndForm.Transparent)

        split.addPane(list0)
        split.addPane(list1)
        split.addPane(list2)
    }

    {
        split := SplitView.create(view, @{0, 0, 300, 0}, vertical: true)
        split.dockStyle = .Right
        split.setFrameForm(FrameWndForm.Square)
        split.setMargin(bottom: 10)

        list0 := createListSimple(split)
        list1 := createListMulti(split)
        list2 := createListSimple(split)
        list0.setFrameForm(FrameWndForm.Transparent)
        list1.setFrameForm(FrameWndForm.Transparent)
        list2.setFrameForm(FrameWndForm.Transparent)

        split.addPane(list0)
        split.addPane(list1)
        split.addPane(list2)
    }
}