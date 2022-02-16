using Core, Gui, Pixel

func createListSimple(view: *Wnd, maxItems = 50)->*List
{
    list := List.createSimple(view, @{5, 5, 250, 300})
    list.setFrameForm(FrameWndForm.Round)
    list.selectionMode = .Multi
    list.iconSize = 16

    loop i: maxItems
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
    list.addColumn("column 0")
    list.addColumn("column 1")
    list.addColumn("column 2")
    list.setFrameForm(FrameWndForm.Round)
    list.selectionMode = .Multi
    list.iconSize = 16

    loop i: 50
    {
        str := Format.toString("item %", i)
        icon := app.theme.getIcon24(ThemeIcons24.Search, 16)
        if i % 2 == 0 icon.clear()
        line := list.createLine(str, icon)
        line.setItem(1, str, icon)
        line.setItem(2, str, icon)
    }

    list.computeLayout()
    return list
}

func createView1(view: *Wnd)
{
    {
        list := createListSimple(view, 10)
        list.frameFlags |= .FocusBorder
        list.selectionMode = .Single
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
        list.disable()
        list.setPosition(@{560, 5, 250, 300})
    }

    {
        list := List.createSimple(view, @{830, 5, 250, 300})
        list.setFrameForm(FrameWndForm.Round)
        list.frameFlags ^= .Borders
        list.selectionMode = .Multi
        list.listFlags |= .HideSelection
        list.horizontalExtent = 500

        list.setVirtualCount(1_000_000)
        list.sigVirtualFill = @(list, line, idx) {
            str := Format.toString("virtual %", idx)
            line.setItem(0, str)
        }
    }

    {
        header := Header.create(view, @{5, 320, 500})
        header.form = .Round
        item := header.addItem("header1")
        item.minWidth = 20
        item = header.addItem("header2")
        item.maxWidth = 100
        header.addItem("header3")
    }

    {
        list := createListMulti(view)
        list.frameFlags |= .FocusBorder
        list.listFlags |= .Checkable
        list.header.headerFlags ^= .Clickable
        list.setPosition(@{5, 400, 400, 300})
        list.ensureVisibleLine(list.getLineCount() - 1)
        list.setColumnWidth(0, 150)

        list.sigSelChanged = @(list) {
            sl := list.getSelectedLine()
            if sl
                Console.printf("selected: %\n", sl.getItem(0).name.raw)
        }
        list.sigCheckChanged = @(list) {
            sl := list.getCheckedLines()
            visit l: sl
                Console.printf("checked: %\n", l.getItem(0).name.raw)
        }
        list.sigRightClick = @(list, pt) {
            sel := list.getSelectedLines()
            visit l: sel
                Console.printf("%\n", l.getItem(0).name.raw)
        }
    }

    {
        list := createListMulti(view)
        list.frameFlags |= .FocusBorder
        list.listFlags |= .AlternateLines | .HorzLines | .VertLines
        list.setFrameForm(FrameWndForm.Square)
        list.setPosition(@{420, 400, 400, 300})

        line0 := list.getLine(0)
        line0.colorBk = Argb.Red
        line0.colorFg = Argb.Blue
        line0.getItem(0).colorFg = Argb.Green
        line0.getItem(0).colorBk = Argb.Blue

        list.header.getItem(0).sizeable = false
        list.header.getItem(0).clickable = false

        list.sigSelChanged = @(list) {
            sel := list.getSelectedLines()
            visit l: sel
                Console.printf("%\n", l.getItem(0).name.raw)
        }
    }
}