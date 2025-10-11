//#global testerror
struct(T) S
{
    #if T == bool
    {
    }
    #else:
        #error("error")
    x:   T
}

#test
{
    var s: S'bool
}
