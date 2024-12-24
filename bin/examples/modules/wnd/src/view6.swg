using Core, Gui, Pixel

func createView6(_tabView: *Tab, view: *Wnd)
{
    {
        let lt = StackLayoutCtrl.create(view, .Top, {5, 5, 200, 200})
        lt.backgroundStyle = .Dialog
        lt.padding         = 5
        PushButton.create(lt, "toto1")
        PushButton.create(lt, "toto2")
        PushButton.create(lt, "toto3")
        lt.computeLayout()
    }

    {
        let lt = StackLayoutCtrl.create(view, .Bottom, {210, 5, 200, 200})
        lt.backgroundStyle = .Dialog
        lt.padding         = 5
        PushButton.create(lt, "toto10")
        PushButton.create(lt, "toto20")
        PushButton.create(lt, "toto30")
        lt.computeLayout()
    }

    {
        let lt = StackLayoutCtrl.create(view, .Left, {420, 5, 400, 40})
        lt.backgroundStyle = .Dialog
        lt.padding         = 5
        PushButton.create(lt, "toto10")
        PushButton.create(lt, "toto20")
        PushButton.create(lt, "toto30")
        lt.computeLayout()
    }

    {
        let lt = StackLayoutCtrl.create(view, .Right, {420, 50, 400, 40})
        lt.backgroundStyle = .Dialog
        lt.padding         = 5
        PushButton.create(lt, "toto10")
        PushButton.create(lt, "toto20")
        PushButton.create(lt, "toto30")
        lt.computeLayout()
    }

    {
        let lt = StackLayoutCtrl.create(view, .Top, {5, 240, 200, 200})
        lt.fitToContent    = true
        lt.padding         = 15
        lt.spacing         = 15
        lt.backgroundStyle = .Window
        lt.style.addStyleSheetColors("wnd_Bk Green")

        let lt1 = StackLayoutCtrl.create(lt, .Top)
        lt1.fitToContent    = true
        lt1.backgroundStyle = .Window
        lt1.style.addStyleSheetColors("wnd_Bk Red")
        PushButton.create(lt1, "toto1")
        PushButton.create(lt1, "toto2")
        PushButton.create(lt1, "toto3")
        lt1.computeLayout()

        let lt2 = StackLayoutCtrl.create(lt, .Top)
        lt2.fitToContent    = true
        lt2.backgroundStyle = .Window
        lt2.style.addStyleSheetColors("wnd_Bk Blue")
        PushButton.create(lt2, "tata1")
        PushButton.create(lt2, "tata2")
        PushButton.create(lt2, "tata3")
        lt2.computeLayout()

        lt.computeLayout()
    }

    {
        let lt = GridLayoutCtrl.create(view, 3, 3, {220, 240, 400, 300})
        lt.backgroundStyle = .Dialog
        lt.padding         = 5
        lt.gridLayoutFlags = .AdaptSizeToContent
        lt.setColRowChild(Label.create(lt, "Label 0"), 0, 0)
        lt.setColRowChild(Label.create(lt, "Label 1"), 1, 0)
        lt.setColRowChild(Label.create(lt, "Label 2"), 2, 0)
        lt.setColRowChild(EditBox.create(lt, "Edit 0"), 0, 1)
        lt.setColRowChild(EditBox.create(lt, "Edit 1"), 1, 1)
        lt.setColRowChild(EditBox.create(lt, "Edit 2"), 2, 1)
        lt.setColRowChild(createListSimple(lt), 0, 2)
        lt.setColRowChild(createTreeSimple(lt), 1, 2)
        lt.setColRowChild(createTreeSimple(lt), 2, 2)
        lt.setColSize(0, 200)
        lt.setColSize(1, 200)
        lt.setColSize(2, 200)
        lt.setRowSize(2, 100)
        lt.computeLayout()
    }
}
