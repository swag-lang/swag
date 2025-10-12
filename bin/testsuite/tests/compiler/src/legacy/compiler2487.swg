using Swag

#[Safety(.All, .Overflow, true)]
//#[PrintBc]
#test
{
    {
        var x: s8 = 1
        x >>= 1
        #assert(1 >> 1 == 0)
        #assert(Swag.U32.Max << 1 == 0xFFFFFFFE)
    }
}
