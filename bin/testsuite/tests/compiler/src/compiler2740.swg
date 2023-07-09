//#[Swag.PrintBc]
#test
{
    {
        var x = 255's16
        var y = cast(u8) x
    }

    {
        var x = 255's32
        var y = cast(u8) x
    }

    {
        var x = 255's64
        var y = cast(u8) x
    }

    {
        var x = 0xFFFF's32
        var y = cast(u16) x
    }

    {
        var x = 0xFFFF's64
        var y = cast(u16) x
    }

    {
        var x = 0xFFFFFFFF's64
        var y = cast(u32) x
    }
}