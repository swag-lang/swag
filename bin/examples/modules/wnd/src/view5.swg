using Core, Gui, Pixel

func createTreeSimple(view: *Wnd, maxItems = 50)->*TreeCtrl
{
    var rng: Random.DefaultRng

    tree := TreeCtrl.createSimple(view, @{5, 5, 250, 300})
    tree.setFrameForm(FrameWndForm.Round)
    tree.selectionMode = .Multi
    tree.iconSize = 16

    loop i: maxItems
    {
        str  := Format.toString("item %", i)
        icon := app.theme.getIcon24(cast(ThemeIcons24) rng.nextU32(0, cast(u32) @countof(ThemeIcons24)))
        line := tree.createLine(null, str, icon)

        loop j: maxItems / 2
        {
            str1  := Format.toString("sub item %", j)
            icon1 := app.theme.getIcon24(cast(ThemeIcons24) rng.nextU32(0, cast(u32) @countof(ThemeIcons24)))
            line1 := tree.createLine(line, str1, icon1)

            loop k: maxItems / 3
            {
                str2  := Format.toString("sub sub item %", k)
                line2 := tree.createLine(line1, str2)
            }
        }
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
        line := tree.createLine(null, str, icon)
        str = Format.toString("sub item %", i)
        line.setItem(1, str)
        line.setItem(2, str)

        loop j: 25
        {
            str1 := Format.toString("sub line %", j)
            line1 := tree.createLine(line, str1)
            str = Format.toString("sub item %", j)
            line1.setItem(1, str)
            line1.setItem(2, str)
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
        btn := PushButton.create(view, "collapse", @{5, 310})
        btn.sigPressed += @|tree|(btn) {
            tree.collapseAll()
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

        /*tree.sigSelChanged += @(tree) {
            sel := tree.getSelectedLines()
            visit l: sel
                Console.printf("%\n", l.getItem(0).name.raw)
        }*/
    }
}