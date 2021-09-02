struct AA
{
    a: u8
    b: u16
    c: u32
    d: u64
}

#assert(@offsetof(AA.a) == 0)
#assert(@offsetof(AA.b) == 2)
#assert(@offsetof(AA.c) == 4)
#assert(@offsetof(AA.d) == 8)
