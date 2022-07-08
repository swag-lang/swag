#global testerror

func(T) toto(v: const [..] T)
{
    #print @typeof(v).fullname
    x := @countof(v)
    @print(x)
}

#test
{
    var x: any
    toto'any(x)
}