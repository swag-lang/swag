using Core, Gui, Pixel

func createTreeSimple(view: *Wnd, maxItems = 50)->*TreeCtrl
{
    tree := TreeCtrl.createSimple(view, @{5, 5, 250, 300})
    tree.setFrameForm(FrameWndForm.Round)
    tree.selectionMode = .Multi
    tree.iconSize = 16

    loop i: maxItems
    {
        str  := Format.toString("item %", i)
        icon := app.theme.getIcon24(ThemeIcons24.Search, 16)
        if i % 2 == 0 icon.clear()
        line := tree.createLine(null, str, icon)

        loop j: maxItems / 2
        {
            str1  := Format.toString("sub item %", j)
            icon1 := app.theme.getIcon24(ThemeIcons24.Search, 16)
            if j % 2 == 0 icon1.clear()
            line1 := tree.createLine(line, str1, icon1)
        }
    }

    tree.computeLayout()
    return tree
}

func createView5(tabView: *Tab, view: *Wnd)
{
    {
        tree := createTreeSimple(view, 10)
        tree.frameFlags |= .FocusBorder
        tree.selectionMode = .Single
        tree.setPosition(@{5, 5, 250, 300})
    }
}