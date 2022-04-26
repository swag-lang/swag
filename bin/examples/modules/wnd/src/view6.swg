using Core, Gui, Pixel

func createView6(tabView: *Tab, view: *Wnd)
{
    {
        lt := LayoutCtrl.create(view, .VerticalStack, @{5, 5, 200, 200})
        PushButton.create(lt, "toto1")
        PushButton.create(lt, "toto2")
        PushButton.create(lt, "toto3")
        lt.computeLayout()
    }
}