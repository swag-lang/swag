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
    func create(parent: *Wnd, position: Math.Rectangle = {}, id: WndId = null)->*PropertyListCtrl
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

            .prop = PropertyList.create(.splitter)

            .splitter.addPane(.list)
            .splitter.addPane(.prop)
        }

        with let filter = EditBox.create(ed, "", {0, 0, 0, 30})
        {
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

    internal mtd fillList()
    {
        var parentLine: *ListLine
        visit &it: prop.categs
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
    mtd addItem(val: any)
    {
        prop.addItem(val)
        fillList()
    }
}
