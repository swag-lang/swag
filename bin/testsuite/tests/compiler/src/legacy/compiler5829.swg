#[Swag.NoInline]
//#[Swag.PrintAsm]
func toto(ptr: [*] s32, idx: u64, val: s32)
{
    ptr[idx] = val
}

#test
{
    var x: [20] s32
    toto(&x[0], 10, 666)
    @assert(x[10] == 666)
}
