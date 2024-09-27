using Libc

func titi(x, y: *s32)->s32
{
    return dref x <=> dref y
}

#test
{
    //discard atexit(&titi)
    //discard at_quick_exit(&titi)
    //exit(666)

    var x = [1, 6, 3, 20, 4, 5]
    qsort(&x, @countof(x), 4, cast(*void) @mkcallback(&titi))
    //foreach v in x
    //    @print(v, " ")
    @assert(x[0] == 1)
    @assert(x[1] == 3)
    @assert(x[2] == 4)
    @assert(x[3] == 5)
    @assert(x[4] == 6)
    @assert(x[5] == 20)
}
