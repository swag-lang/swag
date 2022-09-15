using Core, Pixel, Gui

var app: Application

#main
{
    Memory.DebugAllocator.install(traceLeaks: false)

    surface0 := try app.createSurface(100, 100, 1200, 1000, SurfaceFlags.OverlappedWindow)
    surface0.setTitle("Example 0")
    surface0.setIcon(app.theme.getIcon24(ThemeIcons24.WndMinimize, 20))

    tab := Tab.create(surface0.wnd.view, @{}, id: "bigtab")
    tab.dockStyle = .Center
    tab.barLayout = .Fixed
    tab.setAllPaddings(10)
    tab.style.addStyleSheetColors("tabView_FlatBk $wnd_Bk")

    view0 := Wnd.create'Wnd(tab)
    tab.addItem("Widgets", @{}, view0)
    view1 := Wnd.create'Wnd(tab)
    tab.addItem("ListCtrl", @{}, view1)
    view2 := Wnd.create'Wnd(tab)
    tab.addItem("TreeCtrl", @{}, view2)
    view3 := Wnd.create'Wnd(tab)
    tab.addItem("Color", @{}, view3)
    view4 := Wnd.create'Wnd(tab)
    tab.addItem("RichEditCtrl", @{}, view4)
    view5 := Wnd.create'Wnd(tab)
    tab.addItem("SplitterCtrl", @{}, view5)
    view6 := Wnd.create'Wnd(tab)
    tab.addItem("LayoutCtrl", @{}, view6)

    createView0(tab, view0)
    createView1(tab, view1)
    createView2(tab, view2)
    createView3(tab, view3)
    createView4(tab, view4)
    createView5(tab, view5)
    createView6(tab, view6)

    tab.select(0)
    surface0.show()

    //surface1 := try app.createSurface(100+W+10, 100, W, H, SurfaceFlags.Secondary | SurfaceFlags.SmallCaption | SurfaceFlags.CloseBtn)
    //surface1.show()

    if Env.hasArg("swag.test")
        app.maxRunFrame = 100
    app.run()
}