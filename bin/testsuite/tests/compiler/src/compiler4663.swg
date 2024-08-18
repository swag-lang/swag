#test
{
    {
        const x = 0xFFFFFFFF_FFFFFFFF'u64
        const y = cast(u32) #over x
        const z = cast(u64) y
        #assert cast(s64) z > 0
    }

    {
        const x = 0xFFFFFFFF_FFFFFFFF'u64
        const y = cast(u32) #over x
        const z = cast(u32) y
        #assert cast(s64) z > 0
    }

    {
        const x = 0xFFFFFFFF_FFFFFFFF'u64
        const y = cast(u16) #over x
        const z = cast(u16) y
        #assert cast(s64) z > 0
    }

    {
        const x = 0xFFFFFFFF_FFFFFFFF'u64
        const y = cast(u8) #over x
        const z = cast(u8) y
        #assert cast(s64) z > 0
    }
}