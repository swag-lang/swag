using Gui, Pixel

enum EditMode
{
    Shapes
    Line
}

struct MainWnd
{
    using wnd: Wnd
    icons48:    ImageList
    editWnd:    *EditWnd
    editMode:   EditMode = .Shapes
}

const Icons48 = #load("ressources/icons48.png")

impl MainWnd
{
    mtd createToolbar()
    {
        mtd createBtn(toolbar: *Wnd, n: string, iconIdx: s32)
        {
            b := IconButton.create(toolbar, Icon.from(&icons48, iconIdx), @{0, 0, 56, 64})
            b.name = n
            b.style.addStyleSheetColors("btnIcon_CheckedBk    $btnIcon_HotBk")
            b.style.addStyleSheetColors("btnIcon_CheckedHotBk $btnIcon_HotBk")
            b.iconBtnFlags |= .CheckableToggle
            b.checkedForm = .Bottom
            b.dockStyle = .Left
        }

        toolbar := Wnd.create'Wnd(self, @{0, 0, 0, 64})
        toolbar.dockStyle = .Top
        toolbar.backgroundStyle = .Window
        toolbar.style.addStyleSheetColors("wnd_Bk $wnd_Caption")
        self.createBtn(toolbar, "shape", 0)
        self.createBtn(toolbar, "line", 1)
    }

    func create(parent: *Wnd)->*MainWnd
    {
        res := Wnd.create'MainWnd(parent)
        renderer := parent.getApp().getRenderer()

        img := assume Image.decode(".png", Icons48)
        res.icons48.set(renderer.addImage(img), 48, 48)

        res.createToolbar()
        res.editWnd = EditWnd.create(res)
        res.editWnd.dockStyle = .Center

        return res
    }
}