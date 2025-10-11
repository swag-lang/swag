using Core, Pixel, Gui

var app: Application

var g_Tab:      *Tab
var g_Page:     u32
const MAX_PAGES = 8

#main
{
    let surface0 = try app.createSurface(100, 100, 1200, 1000, SurfaceFlags.OverlappedWindow)
    surface0.setTitle("Example 0")
    surface0.setIcon(app.theme.getIcon24(ThemeIcons24.WndMinimize, 20))

    let tab = Tab.create(surface0.wnd.view, {}, id: "bigtab")
    g_Tab         = tab
    tab.dockStyle = .Center
    tab.barLayout = .Fixed
    tab.setAllPaddings(10)
    tab.style.addStyleSheetColors("tabView_FlatBk $wnd_Bk")

    let view0 = Wnd.create'Wnd(tab)
    tab.addItem("Widgets", {}, view0)
    let view1 = Wnd.create'Wnd(tab)
    tab.addItem("ListCtrl", {}, view1)
    let view2 = Wnd.create'Wnd(tab)
    tab.addItem("TreeCtrl", {}, view2)
    let view3 = Wnd.create'Wnd(tab)
    tab.addItem("Color", {}, view3)
    let view4 = Wnd.create'Wnd(tab)
    tab.addItem("RichEditCtrl", {}, view4)
    let view5 = Wnd.create'Wnd(tab)
    tab.addItem("SplitterCtrl", {}, view5)
    let view6 = Wnd.create'Wnd(tab)
    tab.addItem("LayoutCtrl", {}, view6)
    let view7 = Wnd.create'Wnd(tab)
    tab.addItem("Properties", {}, view7)

    createView0(tab, view0)
    createView1(tab, view1)
    createView2(tab, view2)
    createView3(tab, view3)
    createView4(tab, view4)
    createView5(tab, view5)
    createView6(tab, view6)
    createView7(tab, view7)

    if Env.hasArg("swag.test")
    {
        tab.select(0)
        app.sigFrame += func(app)
        {
            Threading.Thread.sleep(100'ms)
            g_Page = (g_Page + 1) % MAX_PAGES
            if g_Page == 0 do
                Env.exit(0)
            g_Tab.select(g_Page)
        }
    }
    else do
        tab.select(0)
    surface0.show()

    //var surface1 = try app.createSurface(100+W+10, 100, W, H, SurfaceFlags.Secondary | SurfaceFlags.SmallCaption | SurfaceFlags.CloseBtn)
    //surface1.show()

    app.theme.colors.setLight()
    //app.theme.colors.setDark()
    app.themeHasChanged()

    app.run()
}
