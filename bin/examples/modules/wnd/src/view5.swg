using Core, Gui, Pixel

func createTreeSimple(view: *Wnd, maxItems = 50)->*TreeCtrl
{
    tree := TreeCtrl.createSimple(view, @{5, 5, 250, 300})
    tree.setFrameForm(FrameWndForm.Round)
    tree.selectionMode = .Multi
    tree.iconSize = 16

    loop i: maxItems
    {
        str := Format.toString("item %", i)
        icon := app.theme.getIcon24(ThemeIcons24.Search, 16)
        if i % 2 == 0 icon.clear()
        line := tree.createLine(str, icon)
    }

    tree.computeLayout()
    return tree
}

func createTreeMulti(view: *Wnd)->*TreeCtrl
{
    tree := TreeCtrl.createMultiColumns(view, @{5, 5, 250, 300})
    tree.addColumn("column 0")
    tree.addColumn("column 1")
    tree.addColumn("column 2")
    tree.setFrameForm(FrameWndForm.Round)
    tree.selectionMode = .Multi
    tree.iconSize = 16

    loop i: 50
    {
        str := Format.toString("item %", i)
        icon := app.theme.getIcon24(ThemeIcons24.Search, 16)
        if i % 2 == 0 icon.clear()
        line := tree.createLine(str, icon)
        line.setItem(1, str, icon)
        line.setItem(2, str, icon)
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

    {
        tree := createTreeSimple(view)
        tree.setFrameForm(FrameWndForm.Square)
        tree.selectionMode = .None
        tree.setPosition(@{280, 5, 250, 300})
    }

    {
        tree := createTreeSimple(view)
        tree.disable()
        tree.setPosition(@{560, 5, 250, 300})
    }

    {
        tree := createTreeMulti(view)
        tree.frameFlags |= .FocusBorder
        tree.treeFlags |= .Checkable
        tree.header.headerFlags ^= .Clickable
        tree.setPosition(@{5, 400, 400, 300})
        tree.ensureVisibleLine(tree.getLineCount() - 1)
        tree.setColumnWidth(0, 150)

        tree.sigSelChanged = @(tree) {
            sl := tree.getSelectedLine()
            if sl
                Console.printf("selected: %\n", sl.getItem(0).name.raw)
        }
        tree.sigCheckChanged = @(tree) {
            sl := tree.getCheckedLines()
            visit l: sl
                Console.printf("checked: %\n", l.getItem(0).name.raw)
        }
        tree.sigRightClick = @(tree, pt) {
            sel := tree.getSelectedLines()
            visit l: sel
                Console.printf("%\n", l.getItem(0).name.raw)
        }
    }

    {
        tree := createTreeMulti(view)
        tree.frameFlags |= .FocusBorder
        tree.treeFlags |= .AlternateLines | .HorzLines | .VertLines
        tree.setFrameForm(FrameWndForm.Square)
        tree.setPosition(@{420, 400, 400, 300})

        line0 := tree.getLine(0)
        line0.colorBk = Argb.Red
        line0.colorFg = Argb.Blue
        line0.getItem(0).colorFg = Argb.Green
        line0.getItem(0).colorBk = Argb.Blue

        tree.header.getItem(0).sizeable = false
        tree.header.getItem(0).clickable = false

        tree.sigSelChanged = @(tree) {
            sel := tree.getSelectedLines()
            visit l: sel
                Console.printf("%\n", l.getItem(0).name.raw)
        }
    }
}