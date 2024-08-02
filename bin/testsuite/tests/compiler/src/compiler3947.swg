#global testerror "Err0372"
struct V
{
    x, y: s32
}

var G: V{1, 10}

func toto()->&V
{
    return &G
}

//#[Swag.PrintBc]
#test
{
    var x = toto()
    x += 1
}
