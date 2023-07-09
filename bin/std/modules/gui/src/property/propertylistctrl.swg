#global public
using Core, Pixel

struct PropertyListCtrl
{
    using frameWnd: FrameWnd

    splitter:   *SplitterCtrl
    list:       *ListCtrl
    prop:       *PropertyList
}

impl PropertyListCtrl
{
    // Creates a new PropertyList instance
    #[Swag.Discardable]
    func create(parent: *Wnd, position: Math.Rectangle = {}, id: WndId = null)->*PropertyListCtrl
    {
        var ed = Wnd.create'PropertyListCtrl(parent, "", position, id)

        with ed
        {
            .splitter = SplitterCtrl.create(ed, vertical = false)
            .splitter.dockStyle = .Center

            .list = ListCtrl.createSimple(.splitter)
            .list.listFlags.add(.ForceMarginExpandMark)
			.list.sigSelChanged += closure|ed|(list) {
				var sel = list.getFocusLine()
                if !sel
                    return
                list.toggleExpand(sel)
				var item = cast(*PropertyListItem) sel.userData0
				var pos = item.label.getRectIn(ed.prop.edView)
				ed.prop.scrollWnd.setScrollPos(0, pos.y)
			}

            .prop = PropertyList.create(.splitter)

            .splitter.addPane(.list)
            .splitter.addPane(.prop)
        }

        with var filter = EditBox.create(ed, "", {0, 0, 0, 30})
        {
            .dockStyle = .Top
            .setNote("Search")
            .margin.w = 10
            .sigChanged += closure|ed|(fil) {
                ed.prop.setStringFilter(fil.text)
            }
        }

        ed.applyLayout()
        return ed
    }

	private mtd fillList()
	{
		var parentLine: *ListLine
		visit &it: prop.categs
		{
			if it.item1 == 0
				parentLine = null

            var str = it.item0.label.getText()
            var naked = RichString.getNaked(str)
			var line = list.createLine(naked, parentLine = parentLine)
			line.userData0 = it.item0

			if it.item1 == 0
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