#global if #os == Swag.TargetOs.Windows
using Core, Pixel, Gui

const W = 1024
const H = 1024

var app: Gui.Application

#main
{
    surface0 := try app.createSurface(100, 100, W, H, SurfaceFlags.OverlappedWindow)
    surface0.setTitle("Example 0")
    surface0.setIcon(app.theme.getIcon24(ThemeIcons24.WndMinimize, 20))

    tab := Tab.create(surface0.wnd.view, @{})
    tab.dockStyle = .Center
    tab.tabBarLayout = .Fixed
    tab.padding = @{10, 10, 10, 10}
    view0 := Wnd.create'Wnd()
    tab.addView("Widgets", view0)

    createWidgets(view0)
    surface0.show()

    //surface1 := try app.createSurface(100+W+10, 100, W, H, SurfaceFlags.Secondary | SurfaceFlags.SmallCaption | SurfaceFlags.CloseBtn)
    //surface1.show()

    app.run()
}