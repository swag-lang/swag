//#global skip

var g   = 4
var g1: *s32

func titi()->&*s32
{
    return &g1
}

//#[Swag.PrintBc]
func toto()->*s32
{
    return titi()
}

//#[Swag.PrintBc]
#test
{
    g1 = &g
    let a = toto()
    @assert(dref a == 4)
}
