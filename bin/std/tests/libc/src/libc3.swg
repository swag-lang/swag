using Core, Libc

#test
{
    var timer: time_t
    discard time(&timer)

    var ptr = ctime(&timer)
    var a = strlen(ptr)
    @assert(a > 0)
    //Console.print(@mkstring(ptr, a))
}