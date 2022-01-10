#global if #os == Swag.TargetOs.Windows
using Core, Pixel, Gui

const W = 1200
const H = 1000

var app: Gui.Application

#main
{
    surface0 := try app.createSurface(100, 100, W, H, SurfaceFlags.OverlappedWindow)
    surface0.setTitle("Example 0")
    surface0.setIcon(app.theme.getIcon24(ThemeIcons24.WndMinimize, 20))

    tab := Tab.create(surface0.wnd.view, @{}, id: "bigtab")
    tab.dockStyle = .Center
    tab.barLayout = .Fixed
    tab.padding = @{10, 10, 10, 10}
    tab.style.addStyleSheetColors("tabView_FlatBk $wnd_Bk")

    view0 := Wnd.create'Wnd(null, "bigview0")
    tab.addItem("Widgets", @{}, view0)
    view1 := Wnd.create'Wnd(null, "bigview1")
    tab.addItem("List", @{}, view1)

    createView0(view0)
    createView1(view1)
    surface0.show()

    //surface1 := try app.createSurface(100+W+10, 100, W, H, SurfaceFlags.Secondary | SurfaceFlags.SmallCaption | SurfaceFlags.CloseBtn)
    //surface1.show()

    app.run()
}