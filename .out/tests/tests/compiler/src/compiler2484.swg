using swag

#[safety("of", true)]
//#[printbc]
#test
{
    #assert 1'u8 << 8 == 0b100000000
    #semerror x0 := cast(u8) (1'u8 << 8)
    #semerror x1 := 1'u32 << 32
    #semerror x2 := 1'u64 << 64
    #semerror x3 := swag.U32.Max << 1
    #semerror x4 := swag.U64.Max << 1
    #semerror x5 := 1'u32 >> 1
    #semerror x6 := 1'u64 >> 1
}