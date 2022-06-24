//#[Swag.PrintBc]
#[Swag.Optim("", false)]
func toto(x: u32)
{
    var A: [2] s32
    v := A[x >= 32 ? 1 : 0]
}

#test
{
    toto(1)
}