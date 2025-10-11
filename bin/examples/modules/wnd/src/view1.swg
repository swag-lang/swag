using Core, Gui, Pixel

func createListSimple(view: *Wnd, maxItems = 50)->*ListCtrl
{
    let list = ListCtrl.createSimple(view, {5, 5, 250, 300})
    list.setFrameForm(FrameWndForm.Round)
    list.selectionMode = .Multi
    list.iconSize      = 16

    var str: String
    for i in maxItems
    {
        Format.replaceString(&str, "item %", i)
        var icon = app.theme.getIcon24(ThemeIcons24.Search, 16)
        if i % 2 == 0 do
            icon.clear()
        discard list.createLine(str, icon)
    }

    list.computeLayout()
    return list
}

func createListMulti(view: *Wnd, checkable: bool = false)->*ListCtrl
{
    let list = ListCtrl.createMultiColumns(view, {5, 5, 250, 300})
    list.addColumn("column 0")
    list.addColumn("column 1")
    list.addColumn("column 2")
    list.setFrameForm(FrameWndForm.Round)
    list.header.getItem(2).iconMargin = true
    list.selectionMode                = .Multi
    list.iconSize                     = 16

    var str: String
    for i in 50
    {
        Format.replaceString(&str, "item %", i)
        var icon = app.theme.getIcon24(ThemeIcons24.Search, 16)
        if i % 2 == 0 do
            icon.clear()
        let line = list.createLine(str, icon)
        line.canCheck = checkable
        line.setItem(1, str)
        line.setItem(2, str, icon)
    }

    list.computeLayout()
    return list
}

func createView1(_tabView: *Tab, view: *Wnd)
{
    {
        let list = createListSimple(view, 10)
        list.style.setFont(view.getTheme().createDefaultFont(40), .Regular)
        list.frameFlags.add(.FocusBorder)
        list.listFlags.add(.HotTrack)
        list.selectionMode = .Single
        list.setPosition({5, 5, 250, 300})
    }

    {
        let list = createListSimple(view)
        list.setFrameForm(FrameWndForm.Square)
        list.selectionMode = .None
        list.setPosition({280, 5, 250, 300})
    }

    {
        let list = createListSimple(view)
        list.disable()
        list.setPosition({560, 5, 250, 300})
    }

    {
        let list = ListCtrl.createSimple(view, {830, 5, 250, 300})
        list.setFrameForm(FrameWndForm.Round)
        list.frameFlags.remove(.Borders)
        list.selectionMode = .Multi
        list.listFlags |= .HideSelection
        list.horizontalExtent = 500

        list.setVirtualCount(1_000_000)
        list.sigVirtualFill = func(list, line, idx)
        {
            var str = Format.toString("virtual %", idx)
            line.setItem(0, str)
        }
    }

    {
        let header = Header.create(view, {5, 320, 500})
        header.form = .Round
        var item = header.addItem("header1")
        item.minWidth = 20
        item          = header.addItem("header2")
        item.maxWidth = 100
        header.addItem("header3")
    }

    {
        let list = createListMulti(view, checkable: true)
        list.frameFlags.add(.FocusBorder)
        list.header.headerFlags.remove(.Clickable)
        list.setPosition({5, 400, 400, 300})
        list.ensureVisibleLine(list.getLineCount() - 1)
        list.setColumnWidth(0, 150)

        list.sigSelChanged += func(list)
        {
            let sl = list.getSelectedLine()
            if sl do
                Console.printf("selected: %\n", sl.getItem(0).name.raw)
        }
        list.sigCheckChanged += func(list)
        {
            let sl = list.getCheckedLines()
            foreach l in sl do
                Console.printf("checked: %\n", l.getItem(0).name.raw)
        }
        list.sigRightClick += func(list, pt)
        {
            let sel = list.getSelectedLines()
            foreach l in sel do
                Console.printf("%\n", l.getItem(0).name.raw)
        }
    }

    {
        let list = createListMulti(view)
        list.frameFlags.add(.FocusBorder)
        list.listFlags |= .AlternateLines | .HorzLines | .VertLines
        list.setFrameForm(FrameWndForm.Square)
        list.setPosition({420, 400, 400, 300})

        let line0 = list.getLine(0)
        line0.colorBk            = Color.getDarker(Argb.Red, 0.5)
        line0.colorFg            = Color.getLighter(Argb.Yellow, 0.5)
        line0.getItem(0).colorFg = Argb.Yellow
        line0.getItem(0).colorBk = Color.getDarker(Argb.Blue, 0.5)

        list.header.getItem(0).sizeable  = false
        list.header.getItem(0).clickable = false

        list.sigSelChanged += func(list)
        {
            let sel = list.getSelectedLines()
            foreach l in sel do
                Console.printf("%\n", l.getItem(0).name.raw)
        }
    }
}
