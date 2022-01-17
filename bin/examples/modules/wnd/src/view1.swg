using Core, Gui

func createView1(view: *Wnd)
{
    {
        list := List.createSimple(view, @{5, 5, 250, 300})
        list.setFrameForm(FrameWndForm.Round)
        list.isMultiSel = true

        loop i: 50
        {
            str := Format.toString("item%", i)
            icon := app.theme.getIcon24(ThemeIcons24.Search, 16)
            if i % 2 == 0 icon.clear()
            line := list.createLine(str, icon)
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
            str := Format.toString("item%", idx)
            line.setItem(0, str)
        }
    }
}