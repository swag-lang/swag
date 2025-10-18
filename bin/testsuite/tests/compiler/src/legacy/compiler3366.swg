#global testerror "Err0050"

#[Swag.NoCopy]
struct AA { x: s32 }

//#[Swag.PrintBc]
#test
{
    var v:  [4] AA
    var v1: [4] AA
    v = v1
}
