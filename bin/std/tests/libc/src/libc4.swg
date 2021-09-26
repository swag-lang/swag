using Core, Libc

func toto(f: cstring, x: cvarargs)
{
    var az: [128] u8
    var arg: va_list
    va_start(&arg, 0)
    vsprintf(az, f, &arg)
    va_end(&arg)
    @print("libc4 ", cast(cstring) az)
}

#test
{
    toto("%d %f", -1's8, 3.14)
}