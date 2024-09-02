alias LARGE_INTEGER = s64
func toto(ptr: *LARGE_INTEGER)
{
}

func titi()
{
    var x: s64 = 0
    toto(&x)
}