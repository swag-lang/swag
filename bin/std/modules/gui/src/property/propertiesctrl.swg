#global public
using Core, Pixel

#[Swag.EnumFlags]
enum PropertiesCtrlFlags
{
    Zero          = 0
    FindBar               // Add a find bar at the top
    SectionPane           // Display all sections in a pane on the left
    SectionFilter         // Display a section filter bar
}

struct PropertiesCtrl
{
    using frameWnd:     FrameWnd

    splitter:           *SplitterCtrl
    list:               *ListCtrl
    tab:                *Tab
    prop:               *Properties
    propFlags:          PropertiesCtrlFlags
}

impl PropertiesCtrl
{
    // Creates a new Properties instance
    #[Swag.Discardable]
    func create(parent: *Wnd, position: Math.Rectangle = {}, kind = PropertiesKind.List, flags = PropertiesCtrlFlags.SectionPane, id: WndId = null)->*PropertiesCtrl
    {
        let ed = Wnd.create'PropertiesCtrl(parent, "", position, id)

        with ed
        {
            .propFlags = flags

            .splitter           = SplitterCtrl.create(ed, vertical: false)
            .splitter.dockStyle = .Center

            .list = ListCtrl.createSimple(.splitter)
            .list.listFlags.add(.ForceMarginExpandMark)
            .list.sigSelChanged += func|ed|(list)
            {
                let sel = list.getFocusLine()
                if !sel do
                    return
                list.toggleExpand(sel)
                let item = cast(*PropertiesItem) sel.userData0
                let pos  = item.label.getRectIn(ed.prop.edView)
                ed.prop.scrollWnd.setScrollPos(0, pos.y)
            }

            .prop = Properties.create(.splitter, kind: kind)

            .splitter.addPane(.list)
            .splitter.addPane(.prop)

            if !flags.has(.SectionPane) do
                .splitter.showPane(0, false)
        }

        // Filter bar
        if flags.has(.FindBar)
        {
            with let filter = EditBox.create(ed, "")
            {
                .icon      = ed.getTheme().getIcon24(ThemeIcons24.Search, 18)
                .dockStyle = .Top
                .setNote("Search")
                .margin.w = 8
                .sigChanged += func|ed|(fil) { ed.prop.setStringFilter(fil.text); }
                .editBoxFlags.add(.ClearButton)
            }
        }

        // Section filter bar
        if flags.has(.SectionFilter)
        {
            with ed.tab = Tab.create(ed, {0, 0, 0, 30})
            {
                .margin.w  = 12
                .dockStyle = .Top
                .setForm(.PushButton)
                .sigSelChanged += func|ed|(tab) { ed.prop.setCurrentSection(cast(u32) cast(u64) tab.getSelectedItem().userData0); }
            }
        }

        ed.applyLayout()
        return ed
    }

    internal mtd fillSections()
    {
        .list.clear()

        // In the filter bar
        if .tab
        {
            .tab.clear()
            let tabItem = .tab.addItem("All")
            tabItem.userData0 = cast(*void) .prop.sections.count
        }

        var parentLine: *ListLine
        foreach &it, idx in .prop.sections
        {
            let str   = it.item0.label.getText()
            let naked = RichString.getNaked(str)

            // In the filter bar
            if .tab and it.item1 == 0
            {
                let tabItem = .tab.addItem(naked)
                tabItem.userData0 = cast(*void) idx
            }

            // In the left pane
            if it.item1 == 0 do
                parentLine = null

            let line = .list.createLine(naked, parentLine: parentLine)
            line.userData0 = it.item0

            if it.item1 == 0 do
                parentLine = line
        }
    }

    // Add a new struct to display
    mtd addStruct(val: any, category: string = null)
    {
        .prop.addStruct(val, category)
        .fillSections()
    }
}
