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
        ed := Wnd.create'PropertyListCtrl(parent, "", position, id)

        with ed
        {
            .splitter = SplitterCtrl.create(ed, vertical: false)
            .splitter.dockStyle = .Center

            .list = ListCtrl.createSimple(.splitter)
            .prop = PropertyList.create(.splitter)

            .splitter.addPane(.list)
            .splitter.addPane(.prop)
        }

        filter := EditBox.create(ed, "", {0, 0, 0, 30})
        filter.dockStyle = .Top
        filter.setNote("Search")
        filter.margin.w = 10
        filter.sigChanged += closure|ed|(fil) {
            ed.prop.setStringFilter(fil.text)
        }

        ed.applyLayout()
        return ed
    }
}