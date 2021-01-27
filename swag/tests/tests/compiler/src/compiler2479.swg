//#[swag.printbc]
#[swag.safety("of", true)]
#test
{
    {
        x := 0xFFFFFFFF'u32
        #runerror x0 := cast(u8) x
        #runerror x1 := cast(u16) x
    }

    {
        x := 0xFFFFFFFF_FFFFFFFF'u64
        #runerror x0 := cast(u32) x
    }
}