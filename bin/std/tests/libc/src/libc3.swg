using Core, Libc

#test
{
    var timer: time_t
    discard time(&timer)

    let ptr = ctime(&timer)
    let a   = strlen(ptr)
    @assert(a > 0)
}
