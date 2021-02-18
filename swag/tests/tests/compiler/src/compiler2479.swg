//#[swag.printbc]
/*
#[swag.safety("of", true)]
#test
{
    {
        x := 0xFFFF'u16
        x0 := cast(u8) x
    }

    {
        x := 0xFFFFFFFF'u32
        x0 := cast(u8) x
        x1 := cast(u16) x
    }

    {
        x := 0xFFFFFFFF_FFFFFFFF'u64
        x0 := cast(u32) x
        x1 := cast(u8) x
        x2 := cast(u16) x
    }
}*/