#global testerror "Err0179"

enum AA: const [] s32
{
    A = [1, 2]
    B = [10, 20, 30]
}

/*
#assert(@countof(AA.A) == 2)
#assert(@countof(AA.B) == 2)
#assert(AA.A[0] == 1)
#assert(AA.A[1] == 2)
#assert(AA.B[0] == 10)
#assert(AA.B[1] == 20)

#test
{
    var x = AA.A
    @assert(x[0] == 1)
    @assert(x[1] == 2)
}*/