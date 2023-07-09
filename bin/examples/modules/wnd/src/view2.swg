using Core, Gui, Pixel

func createTreeSimple(view: *Wnd, maxItems = 50)->*ListCtrl
{
    var rng = Random.shared()

    var tree = ListCtrl.createSimple(view, {5, 5, 250, 300})
    tree.setFrameForm(FrameWndForm.Round)
    tree.selectionMode = .Multi
    tree.iconSize = 16

    var str: String
    loop i: maxItems
    {
        Format.replaceString(&str, "item %", i)
        var icon = app.theme.getIcon24(cast(ThemeIcons24) rng.nextU32(0, cast(u32) @countof(ThemeIcons24)))
        var line = tree.createLine(str, icon)

        loop j: maxItems / 2
        {
            Format.replaceString(&str, "sub item %", j)
            var icon1 = app.theme.getIcon24(cast(ThemeIcons24) rng.nextU32(0, cast(u32) @countof(ThemeIcons24)))
            var line1 = tree.createLine(str, icon1, parentLine = line)

            loop k: maxItems / 3
            {
                Format.replaceString(&str, "sub sub item %", k)
                discard tree.createLine(str, parentLine = line1)
            }
        }
    }

    tree.computeLayout()
    return tree
}

func createTreeMulti(view: *Wnd)->*ListCtrl
{
    var tree = ListCtrl.createMultiColumns(view, {5, 5, 250, 300})
    tree.addColumn("column 0")
    tree.addColumn("column 1")
    tree.addColumn("column 2")
    tree.setFrameForm(FrameWndForm.Round)
    tree.selectionMode = .Multi
    tree.iconSize = 16
    tree.iconMarginFirstCol = false
    tree.header.getItem(2).iconMargin = true

    var str: String
    loop i: 50
    {
        Format.replaceString(&str, "item %", i)
        var icon = app.theme.getIcon24(ThemeIcons24.Search, 16)
        if i % 2 == 0 icon.clear()
        var line = tree.createLine(str, icon)
        Format.replaceString(&str, "sub item %", i)
        line.setItem(1, str)
        line.setItem(2, str, icon)

        loop j: 25
        {
            Format.replaceString(&str, "sub line %", j)
            var line1 = tree.createLine(str, parentLine = line)
            Format.replaceString(&str, "sub item %", j)
            line1.setItem(1, str)
            line1.setItem(2, str, icon)

            loop k: 10
            {
                Format.replaceString(&str, "XX %", k)
                var line2 = tree.createLine(str, parentLine = line1)
                Format.replaceString(&str, "sub item %", k)
                line2.canCheck = true
                line2.setItem(1, str)
                line2.setItem(2, str, icon)
            }
        }
    }

    tree.computeLayout()
    return tree
}

func createView2(_tabView: *Tab, view: *Wnd)
{
    {
        var tree = createTreeSimple(view, 10)
        tree.frameFlags |= .FocusBorder
        tree.selectionMode = .Single
        tree.setPosition({5, 5, 250, 300})
        var btn = PushButton.create(view, "collapse", {5, 310})
        btn.sigPressed += closure|tree|(btn) {
            tree.collapseAll()
        }
    }

    {
        var tree = createTreeMulti(view)
        tree.frameFlags |= .FocusBorder
        tree.listFlags |= .AlternateLines | .HorzLines | .VertLines
        tree.setFrameForm(FrameWndForm.Square)
        tree.setPosition({420, 5, 600, 300})
        tree.setColumnWidth(0, 200)

        tree.sigSelChanged += func(tree) {
            var sel = tree.getSelectedLines()
            visit l: sel
                Console.printf("%\n", l.getItem(0).name.raw)
        }
    }
}