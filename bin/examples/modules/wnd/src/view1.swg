using Core, Gui

func createListSimple(view: *Wnd)->*List
{
    list := List.createSimple(view, @{5, 5, 250, 300})
    list.setFrameForm(FrameWndForm.Round)
    list.selectionMode = .MultiExtended
    list.iconSize = 16

    loop i: 50
    {
        str := Format.toString("item %", i)
        icon := app.theme.getIcon24(ThemeIcons24.Search, 16)
        if i % 2 == 0 icon.clear()
        line := list.createLine(str, icon)
    }

    list.computeLayout()
    return list
}

func createListMulti(view: *Wnd)->*List
{
    list := List.createMultiColumns(view, @{5, 5, 250, 300})
    list.setFrameForm(FrameWndForm.Round)
    list.selectionMode = .MultiExtended
    list.iconSize = 16

    loop i: 50
    {
        str := Format.toString("item %", i)
        icon := app.theme.getIcon24(ThemeIcons24.Search, 16)
        if i % 2 == 0 icon.clear()
        line := list.createLine(str, icon)
    }

    list.computeLayout()
    return list
}

func createView1(view: *Wnd)
{
    {
        list := createListSimple(view)
        list.focusBorder = true
        list.setPosition(@{5, 5, 250, 300})
    }

    {
        list := createListSimple(view)
        list.setFrameForm(FrameWndForm.Square)
        list.selectionMode = .None
        list.setPosition(@{280, 5, 250, 300})
    }

    {
        list := createListSimple(view)
        list.isDisabled = true
        list.setPosition(@{560, 5, 250, 300})
    }

    {
        list := List.createSimple(view, @{830, 5, 250, 300})
        list.setFrameForm(FrameWndForm.Round)
        list.hasBorders = false
        list.selectionMode = .MultiSimple
        list.hideSelection = true
        list.horizontalExtent = 500

        list.setVirtualCount(1_000_000)
        list.sigVirtualFill = @(line: *ListLine, idx: u32) {
            str := Format.toString("virtual %", idx)
            line.setItem(str)
        }
    }

    {
        header := Header.create(view, @{5, 320, 300})
        header.form = .FlatRound
        header.addItem("header1")
        header.addItem("header2")
        header.addItem("header3")
    }

    {
        list := createListMulti(view)
        list.focusBorder = true
        list.setPosition(@{5, 400, 250, 300})
    }
}