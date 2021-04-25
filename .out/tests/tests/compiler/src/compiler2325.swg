struct AA
{
    x: u64
    y: u16
}

struct BB
{
    v: AA
    z: u32 = 555
}

#assert(@alignof(bool) == 1)
#assert(@alignof(u16) == 2)
#assert(@alignof(u32) == 4)
#assert(@alignof(rune) == 4)
#assert(@alignof(string) == @sizeof(*void))
#assert(@alignof(*void) == @sizeof(*void))
#assert(@alignof(AA) == @sizeof(u64))
#assert(@alignof(BB) == @sizeof(u64))
