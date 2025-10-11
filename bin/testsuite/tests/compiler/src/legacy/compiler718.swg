//

//#[Swag.PrintBc]
//#global testerror
#test
{
    var ptr1: [*] u8 = null
    ptr1 += 1
    @assert(cast(u64) ptr1 == 1)
}
