using Core, Gui, Pixel
using PropertyList

enum Enum
{
    First
    Second
    Third
}

#[Swag.EnumFlags]
enum EnumFlags
{
    #[Description("This is the first value of enum flags")]
    Val1
    #[Description("This is something that needs to be done except if it not has do be done, and this is it.")]
    Val2
    #[Description("This is the value 3 of enum flags")]
    Val3
}

struct AA
{
    tt:  string = "static string"

    #[Category("Categ"), SubCategory("SubCateg"), Name("<b>This is a name<b>: <i>italic<i>"), Description("This is something that needs to be done except if it not has do be done, and this is it.")]
    xu8: u8 = 30

    #[Name("<b>Another name<b>"), EditSlider(0, 1)]
    xf32: f32 = 0.5

    #[Name("<b>Another name<b>")]
    x1:  Math.Vector3{11.5, 22, 33}

    #[Description("This is something that needs to be done except if it not has do be done, and this is it.")]
    xb:  bool = true
    #[Description("This is something that needs to be done.")]
    xb1: bool = true

    #[Description("This is something that needs to be done except if it not has do be done, and this is it.")]
    str: String

    #[Category("Categ1"), Description("This is something that needs to be done except if it not has do be done, and this is it.")]
    value: Enum = Third

    #[Name("<b>Another name<b>")]
    value2: EnumFlags = Val1 | Val3
}

var aa: AA

func createView7(_tabView: *Tab, view: *Wnd)
{
    let lt = PropertyList.create(view, {5, 55, 400, 600})
    aa.str = "String content"
    lt.addItem(aa)

    let lt1 = PropertyListCtrl.create(view, {500, 10, 500, 600})
    lt1.addItem(aa)

    lt.sigChanged += closure|lt1|(lt  )
    {
        lt1.prop.refresh()
    }

    lt1.prop.sigChanged += closure|lt|(lt1 )
    {
        lt.refresh()
    }
}
