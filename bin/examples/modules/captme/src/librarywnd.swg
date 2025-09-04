using Threading, Time

enum PreviewSize
{
    Small
    Medium
    Big
}

enum SortMethod
{
    Recent
    FileSize
    Similar
}

struct LibraryWnd
{
    using wnd:                   Wnd

    main:                        *MainWnd
    view:                        *LibraryView
    listCtrl:                    *ListCtrl
    editZoomBar:                 *Wnd
    zoomBtn:                     *IconButton
    sortBtn:                     *IconButton
    filterBtn:                   *IconButton
    previewSize:                 PreviewSize

    invertSort:                  bool
    sortMethod:                  SortMethod
    showUpToDateLinkedImage:     bool = true
    showLinkedImage:             bool = true
    showUnSavedLinkedImage:      bool = true
    showUnLinkedImage:           bool = true
}

impl LibraryWnd
{
    mtd setPreviewSize(size: PreviewSize)
    {
        me.previewSize = size
        let sizeName = Reflection.getEnumName(PreviewSize, &me.previewSize, false)
        me.zoomBtn.name.set(sizeName)
        me.invalidate()
    }

    mtd setSortMethod(method: SortMethod)
    {
        me.sortMethod = method
        let sortName = Reflection.getEnumName(SortMethod, &me.sortMethod, false)
        me.sortBtn.name.set(sortName)
        me.view.sort()
        me.invalidate()
    }

    mtd setSortOrder(invert: bool)
    {
        me.invertSort = invert
        me.view.sort()
        me.invalidate()
    }

    mtd getCapture()->#null *Capture
    {
        if me.view.focusItem < me.view.items.count:
            return me.view.items[me.view.focusItem].capture
        return null
    }

    mtd requestClose()
    {
        if me.view.thread.isValid()
        {
            me.view.thread.safeForceEnd()
            me.view.thread.wait()
        }
    }

    mtd const hasSelected()->bool
    {
        return !me.view.selModel.isEmpty()
    }

    mtd removeSelected()
    {
        foreach it, i in me.view.items
        {
            if it.filtered:
                continue

            if me.view.selModel.isSelected(cast() i):
                g_Library.removeFile(it.fi.fullname)
        }
    }

    mtd moveSelHome()
    {
        if me.view.items.isEmpty():
            return
        me.view.setFocusItem(0)
        me.view.scrollToFocus()
    }

    mtd moveSelEnd()
    {
        if me.view.items.isEmpty():
            return
        me.view.setFocusItem(cast(u32) me.view.items.count - 1)
        me.view.scrollToFocus()
    }

    mtd moveSelLeft()
    {
        if me.view.items.isEmpty():
            return
        if me.view.focusItem == 0 or me.view.focusItem == Swag.U32.Max:
            return
        me.view.focusItem -= 1
        me.view.setFocusItem(me.view.focusItem)
        me.view.scrollToFocus()
    }

    mtd moveSelRight()
    {
        if me.view.items.isEmpty():
            return
        if me.view.focusItem == me.view.items.count - 1 or me.view.focusItem == Swag.U32.Max:
            return
        me.view.focusItem += 1
        me.view.setFocusItem(me.view.focusItem)
        me.view.scrollToFocus()
    }

    mtd moveSelTop()
    {
        if me.view.items.isEmpty():
            return
        let n = me.view.numImagesPerLine()
        if me.view.focusItem < n or me.view.focusItem == Swag.U32.Max:
            return
        me.view.focusItem -= n
        me.view.setFocusItem(me.view.focusItem)
        me.view.scrollToFocus()
    }

    mtd moveSelBottom()
    {
        if me.view.items.isEmpty():
            return
        let n = me.view.numImagesPerLine()
        if me.view.focusItem >= me.view.items.count - n or me.view.focusItem == Swag.U32.Max:
            return
        me.view.focusItem += n
        me.view.setFocusItem(me.view.focusItem)
        me.view.scrollToFocus()
    }

    mtd fill()
    {
        if me.listCtrl.getLineCount():
            return
        g_Library.onDeleteFile += mtd||(file)
        {
            foreach it, i in me.view.items
            {
                if it.fi.fullname == file
                {
                    me.view.selModel.select(cast() i, false)
                    me.view.items.removeAtOrdered(i)
                    break
                }
            }
        }

        let line = me.listCtrl.createLine("All Captures", Icon.from(&me.main.icons24, 4))
        var item = line.setItem(1, Format.toString("% ", g_Library.files.count))
        item.horzAlign = .Right

        g_Library.files.sort(func(a, b)
        {
            return b.lastWriteTime <=> a.lastWriteTime
        })

        var lastDateTime:  const *DateTime
        var lastLine:      *ListLine
        var lastLineYear:  *ListLine
        var lastLineMonth: *ListLine
        var lastLineDay:   *ListLine

        var icon1 = Icon.from(&me.main.icons24, 3)

        var cptYear, cptMonth, cptDay = 0

        #[Swag.Mixin]
        func fillLastYear()
        {
            if lastLineYear
            {
                item           = lastLineYear.setItem(1, Format.toString("% ", cptYear))
                item.horzAlign = .Right
                cptYear        = 0
            }
        }

        #[Swag.Mixin]
        func fillLastMonth()
        {
            if lastLineMonth
            {
                item           = lastLineMonth.setItem(1, Format.toString("% ", cptMonth))
                item.horzAlign = .Right
                cptMonth       = 0
            }
        }

        #[Swag.Mixin]
        func fillLastDay()
        {
            if lastLineDay
            {
                item           = lastLineDay.setItem(1, Format.toString("% ", cptDay))
                item.horzAlign = .Right
                cptDay         = 0
            }
        }

        foreach fi in g_Library.files
        {
            if !lastDateTime or (fi.lastWriteTime.year != lastDateTime.year)
            {
                fillLastYear()
                lastLine           = me.listCtrl.createLine(Format.toString("%", fi.lastWriteTime.year), icon1)
                lastLine.userData0 = cast() cast(u64) fi.lastWriteTime.year
                lastLineYear       = lastLine
            }

            if !lastDateTime or (fi.lastWriteTime.month != lastDateTime.month)
            {
                fillLastMonth()
                lastLine           = me.listCtrl.createLine(Format.toString("%", DateTime.monthName(fi.lastWriteTime.month)), icon1, lastLineYear)
                lastLine.userData0 = cast() cast(u64) fi.lastWriteTime.year
                lastLine.userData1 = cast() cast(u64) fi.lastWriteTime.month
                lastLineMonth      = lastLine
            }

            if !lastDateTime or (fi.lastWriteTime.day != lastDateTime.day)
            {
                fillLastDay()
                lastLine           = me.listCtrl.createLine(Format.toString("%, %", DateTime.dayOfWeekName(fi.lastWriteTime.dayOfWeek()), fi.lastWriteTime.day), icon1, lastLineMonth)
                lastLine.userData0 = cast() cast(u64) fi.lastWriteTime.year
                lastLine.userData1 = cast() cast(u64) fi.lastWriteTime.month
                lastLine.userData2 = cast() cast(u64) fi.lastWriteTime.day
                lastLineDay        = lastLine
            }

            lastDateTime = &fi.lastWriteTime
            cptYear, cptMonth, cptDay += 1
        }

        fillLastYear()
        fillLastMonth()
        fillLastDay()

        me.listCtrl.computeLayout()
        catch me.view.fill()
    }

    mtd registerActions()
    {
        with me
        {
            .registerAction'ActionLibSort()
            .registerAction'ActionLibFilter()
            .registerAction'ActionLibSel()

            .registerKeyShortcut(.Zero, .Left, ActionLibSel.Left)
            .registerKeyShortcut(.Zero, .Right, ActionLibSel.Right)
            .registerKeyShortcut(.Zero, .Up, ActionLibSel.Up)
            .registerKeyShortcut(.Zero, .Down, ActionLibSel.Down)
            .registerKeyShortcut(.Zero, .Home, ActionLibSel.Home)
            .registerKeyShortcut(.Zero, .End, ActionLibSel.End)
        }
    }

    func create(parent: *MainWnd)->*LibraryWnd
    {
        with let res = Wnd.create'LibraryWnd(parent, "captme.librarywnd")
        {
            .main            = parent
            .dockStyle       = .Center
            .backgroundStyle = .View
            .registerActions()
        }

        with let rightBar = Wnd.create'Wnd(res, {0, 0, 300, 0})
        {
            .dockStyle = .Right
        }

        with let scrollWnd = Wnd.create'ScrollWnd(res)
        {
            .dockStyle = .Center
        }

        with let view = Wnd.create'LibraryView(scrollWnd)
        {
            .libWnd    = res
            .padding   = 8
            .scrollWnd = scrollWnd
            .mainWnd   = parent
        }

        scrollWnd.setView(view)
        res.view = view

        with let list = ListCtrl.createMultiColumns(rightBar)
        {
            .addColumn("name", 200)
            .addColumn("count", 70)
            .listView.style.addStyleSheetColors("listItem_SelectedBk $listItem_FocusSelectedBk")
            .lineHeight = 30
            .listFlags |= .NoMouseEmptySel
            .scrollWnd.scrollWndFlags |= .SmallBar | .ForceVertical | .TransparentBar
            .header.hide()
            .backgroundStyle = .Window
            .listFlags |= .ForceMarginExpandMark
            .dockStyle = .Center
            .sigSelChanged += func|view|(list)
            {
                var sel = list.getSelectedLine()
                if !sel:
                    return
                view.filterYear  = cast(u64) sel.userData0
                view.filterMonth = cast(u64) sel.userData1
                view.filterDay   = cast(u64) sel.userData2
                catch view.fill()
            }
        }
        res.listCtrl = list

        // Top bar
        with res.editZoomBar = StackLayoutCtrl.create(res, .HorzCenter, {0, 0, 300, 32})
        {
            .dockStyle       = .Top
            .backgroundStyle = .Window
        }

        with view.imgSpin = ImageRect.create(res, {4, 4, 24, 24})
        {
            .setMovie(view.getTheme().res.spin)
            .hide()
        }

        // Image Size
        with res.zoomBtn = IconButton.create(res.editZoomBar, res.getTheme().getIcon24(.Search), {0, 0, 120, 32})
        {
            .iconBtnFlags.add(.Center)
            .textPos  = .Right
            .arrowPos = .RightDown
            .sigPressed += func|res|(btn)
            {
                var pos = btn.getSurfaceRect()
                with var menu = MenuCtrl.createPopup(res)
                {
                    .menuCtrlFlags |= .NoKeyShortcuts
                    .addItem(ActionRecentSize.LibSmall)
                    .addItem(ActionRecentSize.LibMedium)
                    .addItem(ActionRecentSize.LibBig)
                }
                discard menu.doModal({pos.horzCenter(), pos.bottom()}, .AnchorTopCenter, autoDestroy: true)
            }
        }

        // Sort
        with res.sortBtn = IconButton.create(res.editZoomBar, res.getTheme().getIcon24(.Sort), {0, 0, 120, 32})
        {
            .iconBtnFlags.add(.Center)
            .textPos  = .Right
            .arrowPos = .RightDown
            .sigPressed += func|res|(btn)
            {
                var pos = btn.getSurfaceRect()
                with var menu = MenuCtrl.createPopup(res)
                {
                    .menuCtrlFlags |= .NoKeyShortcuts
                    .addItem(ActionLibSort.Recent)
                    .addItem(ActionLibSort.FileSize)
                    .addItem(ActionLibSort.Similar)
                    .addSeparator()
                    .addItem(ActionLibSort.Invert)
                }
                discard menu.doModal({pos.horzCenter(), pos.bottom()}, .AnchorTopCenter, autoDestroy: true)
            }
        }

        // Filter
        with res.filterBtn = IconButton.create(res.editZoomBar, res.getTheme().getIcon24(.Filter), {0, 0, 120, 32})
        {
            .name.set("Show")
            .iconBtnFlags.add(.Center)
            .textPos  = .Right
            .arrowPos = .RightDown
            .sigPressed += func|res|(btn)
            {
                var pos = btn.getSurfaceRect()
                with var menu = MenuCtrl.createPopup(res)
                {
                    .menuCtrlFlags |= .NoKeyShortcuts
                    .addItem(ActionLibFilter.UpToDateLinkedImage)
                    .addItem(ActionLibFilter.UnSavedLinkedImage)
                    .addSeparator()
                    .addItem(ActionLibFilter.LinkedImage)
                    .addItem(ActionLibFilter.UnLinkedImage)
                }
                discard menu.doModal({pos.horzCenter(), pos.bottom()}, .AnchorTopCenter, autoDestroy: true)
            }
        }

        res.setPreviewSize(.Small)
        res.setSortMethod(res.sortMethod)
        return res
    }
}
