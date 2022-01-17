using Gui

func createView1(view: *Wnd)
{
    {
        list := List.createSimple(view, @{5, 5, 250, 300})
        list.setFrameForm(FrameWndForm.Round)
        list.isMultiSel = true

        loop 50
        {
            line := list.createLine()
        }

        list.computeLayout()
    }

    {
        list := List.createSimple(view, @{310, 5, 250, 300})
        list.setFrameForm(FrameWndForm.Square)
        list.isMultiSel = true

        list.isVirtual = true
        list.setVirtualCount(1_000_000)
        list.sigVirtualFill = @(line: *ListLine, idx: u32) {
        }
    }
}