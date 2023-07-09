#global testerror
struct V
{
    x, y: s32
}

var G: V{1, 10}

func toto()->ref V
{
    return &G
}

//#[Swag.PrintBc]
#test
{
    var x = toto()
    x += 1
}
