using Core, Libc

#test
{
    var timer: time_t
    discard time(&timer)

    ptr := ctime(&timer)
    a := strlen(ptr)
    @assert(a > 0)
    //Console.print(@mkstring(ptr, a))
}