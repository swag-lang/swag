#global testerror "Err0591"

func(T) toto(v: const [..] T)
{
    #print @typeof(v).fullname
    var x = @countof(v)
    @print(x)
}

#test
{
    var x: any
    toto'any(x)
}