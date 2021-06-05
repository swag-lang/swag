//#[Swag.printbc]
#test
{
    {
        x := 255's16
        y := cast(u8) x
    }

    {
        x := 255's32
        y := cast(u8) x
    }

    {
        x := 255's64
        y := cast(u8) x
    }

    {
        x := 0xFFFF's32
        y := cast(u16) x
    }

    {
        x := 0xFFFF's64
        y := cast(u16) x
    }

    {
        x := 0xFFFFFFFF's64
        y := cast(u32) x
    }
}