using Core, Gui, Pixel

func createView6(tabView: *Tab, view: *Wnd)
{
    {
        lt := LayoutCtrl.create(view, .TopStack, @{5, 5, 200, 200})
        lt.backgroundStyle = .Dialog
        lt.padding = 5
        PushButton.create(lt, "toto1")
        PushButton.create(lt, "toto2")
        PushButton.create(lt, "toto3")
        lt.computeLayout()
    }

    {
        lt := LayoutCtrl.create(view, .BottomStack, @{210, 5, 200, 200})
        lt.backgroundStyle = .Dialog
        lt.padding = 5
        PushButton.create(lt, "toto10")
        PushButton.create(lt, "toto20")
        PushButton.create(lt, "toto30")
        lt.computeLayout()
    }

    {
        lt := LayoutCtrl.create(view, .LeftStack, @{420, 5, 400, 40})
        lt.backgroundStyle = .Dialog
        lt.padding = 5
        PushButton.create(lt, "toto10")
        PushButton.create(lt, "toto20")
        PushButton.create(lt, "toto30")
        lt.computeLayout()
    }

    {
        lt := LayoutCtrl.create(view, .RightStack, @{420, 50, 400, 40})
        lt.backgroundStyle = .Dialog
        lt.padding = 5
        PushButton.create(lt, "toto10")
        PushButton.create(lt, "toto20")
        PushButton.create(lt, "toto30")
        lt.computeLayout()
    }

    {
        lt := LayoutCtrl.create(view, .TopStackAutoSize, @{5, 240, 200, 200})
        lt.padding = 15
        lt.spacing = 15
        lt.backgroundStyle = .Window
        lt.style.addStyleSheetColors("wnd_Bk Green")

        lt1 := LayoutCtrl.create(lt, .TopStackAutoSize)
        lt1.backgroundStyle = .Window
        lt1.style.addStyleSheetColors("wnd_Bk Red")
        PushButton.create(lt1, "toto1")
        PushButton.create(lt1, "toto2")
        PushButton.create(lt1, "toto3")
        lt1.computeLayout()

        lt2 := LayoutCtrl.create(lt, .TopStackAutoSize)
        lt2.backgroundStyle = .Window
        lt2.style.addStyleSheetColors("wnd_Bk Blue")
        PushButton.create(lt2, "tata1")
        PushButton.create(lt2, "tata2")
        PushButton.create(lt2, "tata3")
        lt2.computeLayout()

        lt.computeLayout()
    }
}