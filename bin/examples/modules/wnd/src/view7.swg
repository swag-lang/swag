using Core, Gui, Pixel
using Property

enum Enum
{
    First
    Second
    Third
}

struct AA
{
    #[Name("<b>This is a name<b>: <i>italic<i>")]
    #[Description("This is something that needs to be done except if it not has do be done, and this is it.")]
    xu8:  u8 = 30

    #[Name("<b>Another name<b>")]
    xf32: f32 = 10//Math.Vector3{11.5, 22, 33}

    #[Description("This is something that needs to be done.")]
    xb: bool = true

    #[Description("This is something that needs to be done except if it not has do be done, and this is it.")]
    str: String

    value: Enum = Third
}

var aa: AA

func createView7(tabView: *Tab, view: *Wnd)
{
    lt := PropertyCtrl.create(view, @{5, 5, 400, 600})
    aa.str = "String content"
    lt.addItem(aa)
}