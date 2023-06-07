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
    #[Description("This is something that needs to be done 1.")]
    Val1
    #[Description("This is something that needs to be done except if it not has do be done, and this is it.")]
    Val2
    #[Description("This is something that needs to be done 3.")]
    Val3
}

struct AA
{
    #[Category("Categ")]
	#[SubCategory("SubCateg")]
    #[Name("<b>This is a name<b>: <i>italic<i>")]
    #[Description("This is something that needs to be done except if it not has do be done, and this is it.")]
    xu8:  u8 = 30

    #[Name("<b>Another name<b>")]
    xf32: f32 = 10//Math.Vector3{11.5, 22, 33}

    #[Name("<b>Another name<b>")]
    x1: Math.Vector3{11.5, 22, 33}

    #[Description("This is something that needs to be done except if it not has do be done, and this is it.")]
    xb: bool = true
    #[Description("This is something that needs to be done.")]
    xb1: bool = true

    #[Description("This is something that needs to be done except if it not has do be done, and this is it.")]
    str: String

	#[Category("Categ1")]
    value:  Enum = Third

    #[Name("<b>Another name<b>")]
    value2: EnumFlags = Val1 | Val3
}

var aa: AA

func createView7(_tabView: *Tab, view: *Wnd)
{
    lt := PropertyList.create(view, {5, 55, 400, 600})
    aa.str = "String content"
    lt.addItem(aa)

    {
        ed := EditBox.create(view, "", {0, 0, 200})
        ed.setNote("Search")
        ed.sigChanged += closure|lt|(ed) {
            lt.setStringFilter(ed.text)
        }
    }

    {
        ed := PropertyListCtrl.create(view, {500, 10, 500, 600})
        ed.addItem(aa)
    }

}