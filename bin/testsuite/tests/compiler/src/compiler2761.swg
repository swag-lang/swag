enum AA : const [2, 2] s32
{
    A = [[1, 2], [1, 2]]
    B = [[10, 20], [10, 20]]
}

#assert @countof(AA.A) == 2
#assert @countof(AA.B) == 2
