enum AA: const [..] s32
{
    A = @[1, 2, 3, 4]
    B = @[10, 20, 30]
}

#assert @countof(AA.A) == 4
#assert @countof(AA.B) == 3
/*#assert AA.A[0, 0] == 1
#assert AA.A[1, 1] == 2
#assert AA.B[0, 0] == 10
#assert AA.B[1, 1] == 20*/

#test
{
    x := AA.A
    @assert(@countof(x) == 4)
    @assert(x[0] == 1)
    @assert(x[1] == 2)
    @assert(x[2] == 3)
    @assert(x[3] == 4)

    y := AA.B
    @assert(@countof(y) == 3)
    @assert(y[0] == 10)
    @assert(y[1] == 20)
    @assert(y[2] == 30)
}