using Core, Gui, Pixel

func createTreeSimple(view: *Wnd, maxItems = 50)->*ListCtrl
{
    var rng: Random.DefaultRng

    tree := ListCtrl.createSimple(view, @{5, 5, 250, 300})
    tree.setFrameForm(FrameWndForm.Round)
    tree.selectionMode = .Multi
    tree.iconSize = 16

    loop i: maxItems
    {
        str  := Format.toString("item %", i)
        icon := app.theme.getIcon24(cast(ThemeIcons24) rng.nextU32(0, cast(u32) @countof(ThemeIcons24)))
        line := tree.createLine(str, icon)

        loop j: maxItems / 2
        {
            str1  := Format.toString("sub item %", j)
            icon1 := app.theme.getIcon24(cast(ThemeIcons24) rng.nextU32(0, cast(u32) @countof(ThemeIcons24)))
            line1 := tree.createLine(str1, icon1, parentLine: line)

            loop k: maxItems / 3
            {
                str2  := Format.toString("sub sub item %", k)
                line2 := tree.createLine(str2, parentLine: line1)
            }
        }
    }

    tree.computeLayout()
    return tree
}

func createTreeMulti(view: *Wnd)->*ListCtrl
{
    tree := ListCtrl.createMultiColumns(view, @{5, 5, 250, 300})
    tree.addColumn("column 0")
    tree.addColumn("column 1")
    tree.addColumn("column 2")
    tree.setFrameForm(FrameWndForm.Round)
    tree.selectionMode = .Multi
    tree.iconSize = 16
    tree.header.getItem(2).iconMargin = true

    loop i: 50
    {
        str := Format.toString("item %", i)
        icon := app.theme.getIcon24(ThemeIcons24.Search, 16)
        if i % 2 == 0 icon.clear()
        line := tree.createLine(str, icon)
        str = Format.toString("sub item %", i)
        line.setItem(1, str)
        line.setItem(2, str, icon)

        loop j: 25
        {
            str1 := Format.toString("sub line %", j)
            line1 := tree.createLine(str1, parentLine: line)
            str = Format.toString("sub item %", j)
            line1.setItem(1, str)
            line1.setItem(2, str, icon)

            loop k: 10
            {
                str2  := Format.toString("XX %", k)
                line2 := tree.createLine(str2, parentLine: line1)
                str = Format.toString("sub item %", k)
                line2.setItem(1, str)
                line2.setItem(2, str, icon)
            }
        }
    }

    tree.computeLayout()
    return tree
}

func createView2(tabView: *Tab, view: *Wnd)
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
        tree.listFlags |= .AlternateLines | .HorzLines | .VertLines | .Checkable
        tree.setFrameForm(FrameWndForm.Square)
        tree.setPosition(@{420, 5, 600, 300})
        tree.setColumnWidth(0, 200)

        tree.sigSelChanged += @(tree) {
            sel := tree.getSelectedLines()
            visit l: sel
                Console.printf("%\n", l.getItem(0).name.raw)
        }
    }
}