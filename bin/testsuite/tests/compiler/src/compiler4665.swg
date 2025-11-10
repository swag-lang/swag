#test
{
    {
        const x = 0x1_FFFFFFFF's64
        const y = cast #wrap (s32) x
        #assert(cast(s16) y < 0)
    }

    {
        const x = 0x1_FFFF's64
        const y = cast #wrap (s16) x
        #assert(cast(s8) y < 0)
    }
}
