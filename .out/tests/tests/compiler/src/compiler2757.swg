enum AA: const [..] s32
{
    A = @[1, 2]
    B = @[10, 20, 40]
}

#assert @countof(AA.A) == 2
#assert @countof(AA.B) == 3
