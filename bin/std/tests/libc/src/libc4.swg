using Core, Libc

func toto(az: *u8, f: cstring, x: cvarargs)
{
    var arg: va_list
    va_start(&arg, 0)
    vsprintf(az, f, &arg)
    va_end(&arg)
}

#test
{
    var az0: [128] u8
    var az1: [128] u8

    toto(az0, "%d %f", -1's8, 3.14)
    sprintf(az1, "%d %f", -1's8, 3.14)

    @assert(cast(cstring) az0 == cast(cstring) az1)
}
