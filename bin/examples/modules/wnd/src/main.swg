using Core, Pixel, Gui

var app: Application

#main
{
    surface0 := try app.createSurface(100, 100, 1200, 1000, SurfaceFlags.OverlappedWindow)
    surface0.setTitle("Example 0")
    surface0.setIcon(app.theme.getIcon24(ThemeIcons24.WndMinimize, 20))

    tab := Tab.create(surface0.wnd.view, @{}, id: "bigtab")
    tab.dockStyle = .Center
    tab.barLayout = .Fixed
    tab.padding = @{10, 10, 10, 10}
    tab.style.addStyleSheetColors("tabView_FlatBk $wnd_Bk")

    view0 := Wnd.create'Wnd(tab, "bigview0")
    tab.addItem("Widgets", @{}, view0)
    view1 := Wnd.create'Wnd(tab, "bigview1")
    tab.addItem("List", @{}, view1)

    createView0(view0)
    createView1(view1)
    tab.select(1)
    surface0.show()

    //surface1 := try app.createSurface(100+W+10, 100, W, H, SurfaceFlags.Secondary | SurfaceFlags.SmallCaption | SurfaceFlags.CloseBtn)
    //surface1.show()

    app.run()
}