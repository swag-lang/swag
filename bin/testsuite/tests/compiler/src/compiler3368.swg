#global testerror "Err0107"

#[Swag.NoCopy]
struct AA { x: s32; }

func toto()->[1] AA
{
    return [{1}]
}

//#[Swag.PrintBc]
#test
{
    var v: [1] AA
    v = toto()
}