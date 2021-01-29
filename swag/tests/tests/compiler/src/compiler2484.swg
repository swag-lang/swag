using swag

#[safety("of", true)]
//#[printbc]
#test
{
    #assert 1'u8 << 8 == 0b100000000
    #semerror x0 := cast(u8) (1'u8 << 8)
    #semerror x1 := 1'u32 << 32
    #semerror x2 := 1'u64 << 64
}