using Swag

#[safety("overflow", true)]
//#[printbc]
#test
{
    #semerror x0 := 1'u8 << 8
    #semerror x1 := 1'u32 << 32
    #semerror x2 := 1'u64 << 64
    #semerror x3 := Swag.U32.Max << 1
    #semerror x4 := Swag.U64.Max << 1
    #semerror x5 := 1'u32 >> 1
    #semerror x6 := 1'u64 >> 1
}