enum AA: const [..] s32
{
    A = [1, 2, 3, 4]
    B = [10, 20, 30]
}

#assert(AA.A[0] == 1)
#assert(AA.A[1] == 2)
#assert(AA.B[2] == 30)