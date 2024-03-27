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

#assert(@offsetof(AA.x) == 0)
#assert(@offsetof(AA.y) == @sizeof(u64))

#assert(@offsetof(BB.v) == 0)
#assert(@offsetof(BB.z) == 2 * @sizeof(u64))
