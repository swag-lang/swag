using Core, Gui, Pixel

func createView6(tabView: *Tab, view: *Wnd)
{
    {
        lt := StackLayoutCtrl.create(view, .Top, @{5, 5, 200, 200})
        lt.backgroundStyle = .Dialog
        lt.padding = 5
        PushButton.create(lt, "toto1")
        PushButton.create(lt, "toto2")
        PushButton.create(lt, "toto3")
        lt.computeLayout()
    }

    {
        lt := StackLayoutCtrl.create(view, .Bottom, @{210, 5, 200, 200})
        lt.backgroundStyle = .Dialog
        lt.padding = 5
        PushButton.create(lt, "toto10")
        PushButton.create(lt, "toto20")
        PushButton.create(lt, "toto30")
        lt.computeLayout()
    }

    {
        lt := StackLayoutCtrl.create(view, .Left, @{420, 5, 400, 40})
        lt.backgroundStyle = .Dialog
        lt.padding = 5
        PushButton.create(lt, "toto10")
        PushButton.create(lt, "toto20")
        PushButton.create(lt, "toto30")
        lt.computeLayout()
    }

    {
        lt := StackLayoutCtrl.create(view, .Right, @{420, 50, 400, 40})
        lt.backgroundStyle = .Dialog
        lt.padding = 5
        PushButton.create(lt, "toto10")
        PushButton.create(lt, "toto20")
        PushButton.create(lt, "toto30")
        lt.computeLayout()
    }

    {
        lt := StackLayoutCtrl.create(view, .TopAutoSize, @{5, 240, 200, 200})
        lt.padding = 15
        lt.spacing = 15
        lt.backgroundStyle = .Window
        lt.style.addStyleSheetColors("wnd_Bk Green")

        lt1 := StackLayoutCtrl.create(lt, .TopAutoSize)
        lt1.backgroundStyle = .Window
        lt1.style.addStyleSheetColors("wnd_Bk Red")
        PushButton.create(lt1, "toto1")
        PushButton.create(lt1, "toto2")
        PushButton.create(lt1, "toto3")
        lt1.computeLayout()

        lt2 := StackLayoutCtrl.create(lt, .TopAutoSize)
        lt2.backgroundStyle = .Window
        lt2.style.addStyleSheetColors("wnd_Bk Blue")
        PushButton.create(lt2, "tata1")
        PushButton.create(lt2, "tata2")
        PushButton.create(lt2, "tata3")
        lt2.computeLayout()

        lt.computeLayout()
    }

    {
        lt := GridLayoutCtrl.create(view, 5, 3, @{220, 240, 400, 300})
        lt.backgroundStyle = .Dialog
        lt.padding = 5
        btn0 := PushButton.create(lt, "toto10")
        btn1 := PushButton.create(lt, "toto20")
        lt.setColRowChild(btn0, 0, 0, 1, 3)
        lt.setColRowChild(btn1, 1, 0, 4, 3)
        lt.computeLayout()
    }
}