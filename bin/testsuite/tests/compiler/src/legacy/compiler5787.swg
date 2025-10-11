struct A
{
    x:   s32
}

var G: A
func toto()->*A
{
    return &G
}

#[Swag.Macro, Swag.Warning("Wrn0006", Swag.WarningLevel.Disable)]
func assert(expr: bool, message: string = null, loc = #callerlocation)
{
    if !expr do
        @panic(message, loc)
}

//#[Swag.PrintGenBc]
func toto1()
{
    let n = toto()
    assert(n and n.x)
    titi(n)
}

func titi(a: *A) {}
