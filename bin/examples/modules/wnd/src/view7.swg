using Core, Gui, Pixel
using Properties

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
    #[Description("This is something that needs to be done except if it not has: be done, and this is it.")]
    Val2
    #[Description("This is the value 3 of enum flags")]
    Val3
}

struct SubStruct
{
    #[Description("This is a custom color")]
    color: Color
}

struct AA
{
    v4b:     Array'String
    v4c:     Array'String

    tt:      string = "static string"
    v4a:     [4] Math.Vector2

    #[Category("Category"), Name("This is a name: <i>italic<i>"), Description("This is <url>something<url> that needs to be done except if it not has: be done, and this is it.")]
    xu8: u8 = 30

    #[Unit("ms")]
    degs: f32 = 0.5

    #[Name("Another name"), SliderMinMax(0, 1), EditSize(250)]
    xf32: f32 = 0.5

    struct0: SubStruct

    #[Description("This is a custom color")]
    color: Color

    #[Name("Vector2")]
    v2:  Math.Vector2{11.5, 22}

    #[Name("Vector3"), Description("This is a vector3.")]
    v3:  Math.Vector3{11.5, 22, 33}

    v4:       Math.Vector4{11.5, 22, 33}
    pt:       Math.Point{11.5, 22}
    rect:     Math.Rectangle{1, 2, 3, 4}

    #[Description("This is something that needs to be done except if it not has: be done, and this is it.")]
    xb:  bool = true
    #[Description("This is something that needs to be done.")]
    xb1: bool = true

    #[Description("This is something that needs to be done except if it not has: be done, and this is it.")]
    str: String

    #[Description("This is something that needs to be done except if it not has: be done, and this is it.")]
    value: Enum = Third

    #[Name("Another name")]
    value2: EnumFlags = Val1 | Val3
}

var aa: AA

func createView7(_tabView: *Tab, view: *Wnd)
{
    let lt = Properties.create(view, {5, 55, 480, 600}, .Grid)
    aa.str = "String content"
    aa.v4b.add("toto")
    aa.v4b.add("titi")
    aa.v4c.add("toto")
    aa.v4c.add("titi")
    aa.v4c.add("tata")
    lt.addStruct(aa, "Section")

    let lt1 = PropertiesCtrl.create(view, {500, 10, 600, 600}, flags: .SectionFilter | .SectionPane | .FindBar)
    lt1.addStruct(aa, "Section 0")
    lt1.addStruct(aa, "Section 1")
    lt1.addStruct(aa, "Section 2")
    lt1.splitter.setPaneSize(0, 150)

    lt.sigChanged += func|lt1|(lt, item) { lt1.prop.refresh(); }
    lt1.prop.sigChanged += func|lt|(lt1, item) { lt.refresh(); }

    let btn = PushButton.create(view, "show/hide", {500, 650, 600})
    btn.sigPressed += func|lt1|(btn) { lt1.splitter.showPane(0, lt1.splitter.isPaneVisible(0) ? false : true); }
}
