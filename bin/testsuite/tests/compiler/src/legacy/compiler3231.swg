var G: s32

interface IWnd
{
    func titi(me, x: s32);
}

struct Wnd {}

struct Dialog
{
    using wnd: Wnd
}

struct MessageDlg
{
    using dlg: Dialog
}

impl IWnd for Wnd
{
    mtd impl titi(vv: s32)
    {
        G += 2
    }
}

impl IWnd for MessageDlg
{
    mtd impl titi(vv: s32)
    {
        G += 1
        me.dlg.IWnd.titi(vv)
    }
}

#test
{
    var x: MessageDlg
    x.IWnd.titi(0)
    @assert(G == 3)
}
