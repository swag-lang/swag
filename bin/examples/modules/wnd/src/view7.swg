using Core, Gui, Pixel

struct AA
{
    xu8:  u8 = 30
    xf32: f32 = 10//Math.Vector3{11.5, 22, 33}
}

var aa: AA

func createView7(tabView: *Tab, view: *Wnd)
{
    lt := PropertyCtrl.create(view, @{5, 5, 400, 600})
    lt.addItem(aa)
}