using Core, Gui, Pixel

func createView5(_tabView: *Tab, view: *Wnd)
{
    {
        btn := PushButton.create(view, "AnchorR", {600, 400})
        btn.anchorStyle = .X
    }

    {
        btn := PushButton.create(view, "AnchorRB", {600, 440})
        btn.anchorStyle = .X | .Y
    }

    {
        btn := PushButton.create(view, "AnchorW", {600, 480})
        btn.anchorStyle = .Width
    }

    {
        btn := PushButton.create(view, "AnchorH", {600, 520})
        btn.anchorStyle = .Height
    }

    {
        split := SplitterCtrl.create(view, {10, 10, 300, 300}, vertical = true)
        split.setFrameForm(.Round)

        list0 := createListSimple(split)
        list0.setFrameForm(.Transparent)
        list1 := createListSimple(split)
        list1.setFrameForm(.Transparent)

        split.addPane(list0)
        split.addPane(list1)
    }

    {
        split := SplitterCtrl.create(view, {320, 10, 400, 300}, vertical = false)
        split.frameFlags |= .FocusBorder
        split.setFrameForm(.Square)

        ed := RichEditCtrl.create(split, {})
        ed.setRichEdFlags(.WordWrap)
        ed.setForm(.Edit)
        ed.addText("coucou la foule comment ça va ? ça va ! et même très bien ")

        list1 := createListMulti(split)
        list1.setFrameForm(.Transparent)

        split.addPane(ed)
        split.addPane(list1)
    }

    {
        split := SplitterCtrl.create(view, {10, 320, 600, 300}, vertical = false)
        split.dockStyle = .Bottom
        split.setFrameForm(.Square)

        list0 := createListSimple(split)
        list1 := createListMulti(split)
        list2 := createListSimple(split)
        list0.setFrameForm(.Transparent)
        list1.setFrameForm(.Transparent)
        list2.setFrameForm(.Transparent)

        split.addPane(list0)
        pane1 := split.addPane(list1, 300)
        pane1.tryPreserveSize = true
        pane1.maxSize = 300

        pane2 := split.addPane(list2, 300)
        pane2.tryPreserveSize = true
    }

    {
        split := SplitterCtrl.create(view, {0, 0, 300, 0}, vertical = true)
        split.dockStyle = .Right
        split.setFrameForm(.Square)
        split.setMargin(bottom = 10)

        list0 := createListSimple(split)
        list1 := createListMulti(split)
        list2 := createListSimple(split)
        list0.setFrameForm(.Transparent)
        list1.setFrameForm(.Transparent)
        list2.setFrameForm(.Transparent)

        split.addPane(list0)
        split.addPane(list1)
        split.addPane(list2)
    }
}