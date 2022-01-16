using Gui

func createView1(view: *Wnd)
{
    {
        list := List.createSimple(view, @{5, 5, 400, 500})
        list.setForm(ListForm.Round)
    }

    {
        list := List.createSimple(view, @{410, 5, 400, 500})
    }
}