using Core, Pixel, Gui

var app: Application

#main
{
    dbg := Memory.DebugAllocator.create()
    cxt := dref @getcontext()
    cxt.allocator = dbg
    @setcontext(cxt)

    surface0 := try app.createSurface(100, 100, 1200, 1000, SurfaceFlags.OverlappedWindow)
    surface0.setTitle("Example 0")
    surface0.setIcon(app.theme.getIcon24(ThemeIcons24.WndMinimize, 20))

    tab := Tab.create(surface0.wnd.view, @{}, id: "bigtab")
    tab.dockStyle = .Center
    tab.barLayout = .Fixed
    tab.setAllPaddings(10)
    tab.style.addStyleSheetColors("tabView_FlatBk $wnd_Bk")

    view0 := Wnd.create'Wnd(tab, "bigview0")
    tab.addItem("Widgets", @{}, view0)
    view1 := Wnd.create'Wnd(tab, "bigview1")
    tab.addItem("ListCtrl", @{}, view1)
    view2 := Wnd.create'Wnd(tab, "bigview2")
    tab.addItem("SplitterCtrl", @{}, view2)
    view3 := Wnd.create'Wnd(tab, "bigview3")
    tab.addItem("Color", @{}, view3)
    view4 := Wnd.create'Wnd(tab, "bigview4")
    tab.addItem("RichEditCtrl", @{}, view4)

    createView0(tab, view0)
    createView1(tab, view1)
    createView2(tab, view2)
    createView3(tab, view3)
    createView4(tab, view4)
    tab.select(0)
    surface0.show()

    //surface1 := try app.createSurface(100+W+10, 100, W, H, SurfaceFlags.Secondary | SurfaceFlags.SmallCaption | SurfaceFlags.CloseBtn)
    //surface1.show()

    app.run()
}