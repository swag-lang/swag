enum AA: const [2, 2] s32
{
    A = @[[1, 2], [1, 2]]
    B = @[[10, 20], [10, 20]]
}

#assert @countof(AA.A) == 2
#assert @countof(AA.B) == 2
/*#assert AA.A[0, 0] == 1
#assert AA.A[1, 1] == 2
#assert AA.B[0, 0] == 10
#assert AA.B[1, 1] == 20*/
