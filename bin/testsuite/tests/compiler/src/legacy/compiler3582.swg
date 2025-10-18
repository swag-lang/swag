#global testerror "Err0098"

func toto(b: bool, h, s, l: f32 = -2) {}

//#[Swag.PrintBc]
#test
{
    var x: bool
    x.toto(hue: 0)
}
