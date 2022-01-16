using Gui

func createView1(view: *Wnd)
{
    {
        list := List.createSimple(view, @{5, 5, 250, 300})
        list.setForm(ListForm.Round)
        loop 50
        {
            line := list.createLine()
        }

        list.computeLayout()
    }

    {
        list := List.createSimple(view, @{410, 5, 400, 500})
    }
}