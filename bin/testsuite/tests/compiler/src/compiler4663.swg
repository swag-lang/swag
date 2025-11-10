#test
{
    {
        const x = 0xFFFFFFFF_FFFFFFFF'u64
        const y = cast #wrap (u32) x
        const z = cast(u64) y
        #assert(cast(s64) z > 0)
    }

    {
        const x = 0xFFFFFFFF_FFFFFFFF'u64
        const y = cast #wrap (u32) x
        const z = cast(u32) y
        #assert(cast(s64) z > 0)
    }

    {
        const x = 0xFFFFFFFF_FFFFFFFF'u64
        const y = cast #wrap (u16) x
        const z = cast(u16) y
        #assert(cast(s64) z > 0)
    }

    {
        const x = 0xFFFFFFFF_FFFFFFFF'u64
        const y = cast #wrap (u8) x
        const z = cast(u8) y
        #assert(cast(s64) z > 0)
    }
}
