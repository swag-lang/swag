#global public
using Core, Pixel

struct PropertyListCtrl
{
    using frameWnd:     FrameWnd

    splitter:           *SplitterCtrl
    list:               *ListCtrl
    prop:               *PropertyList
}

impl PropertyListCtrl
{
    // Creates a new PropertyList instance
    #[Swag.Discardable]
    func create(parent: *Wnd, position: Math.Rectangle = {}, kind = PropertyListKind.List, id: WndId = null)->*PropertyListCtrl
    {
        let ed = Wnd.create'PropertyListCtrl(parent, "", position, id)

        with ed
        {
            .splitter           = SplitterCtrl.create(ed, vertical: false)
            .splitter.dockStyle = .Center

            .list = ListCtrl.createSimple(.splitter)
            .list.listFlags.add(.ForceMarginExpandMark)
            .list.sigSelChanged += closure|ed|(list)
            {
                let sel = list.getFocusLine()
                if !sel do
                    return
                list.toggleExpand(sel)
                let item = cast(*PropertyListItem) sel.userData0
                let pos  = item.label.getRectIn(ed.prop.edView)
                ed.prop.scrollWnd.setScrollPos(0, pos.y)
            }

            .prop = PropertyList.create(.splitter, kind: kind)

            .splitter.addPane(.list)
            .splitter.addPane(.prop)
        }

        with let filter = EditBox.create(ed, "", {0, 0, 0, 30})
        {
            .icon = ed.getTheme().getIcon24(ThemeIcons24.Search, 18)
            .dockStyle = .Top
            .setNote("Search")
            .margin.w = 10
            .sigChanged += closure|ed|(fil)
            {
                ed.prop.setStringFilter(fil.text)
            }
        }

        ed.applyLayout()
        return ed
    }

    internal mtd fillSections()
    {
        list.clear()

        var parentLine: *ListLine
        visit &it: prop.sections
        {
            if it.item1 == 0 do
                parentLine = null

            let str   = it.item0.label.getText()
            let naked = RichString.getNaked(str)
            let line  = list.createLine(naked, parentLine: parentLine)
            line.userData0 = it.item0

            if it.item1 == 0 do
                parentLine = line
        }
    }

    // Add a new struct to display
    mtd addStruct(val: any, category: string = null)
    {
        prop.addStruct(val, category)
        fillSections()
    }
}
