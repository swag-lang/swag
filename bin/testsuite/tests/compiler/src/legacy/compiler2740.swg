//#[Swag.PrintBc]
#test
{
    {
        let x = 255's16
        let y = cast(u8) x
    }

    {
        let x = 255's32
        let y = cast(u8) x
    }

    {
        let x = 255's64
        let y = cast(u8) x
    }

    {
        let x = 0xFFFF's32
        let y = cast(u16) x
    }

    {
        let x = 0xFFFF's64
        let y = cast(u16) x
    }

    {
        let x = 0xFFFFFFFF's64
        let y = cast(u32) x
    }
}
