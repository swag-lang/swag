alias HANDLE = const *void

func toto(ptr: *HANDLE)
{
}

func toto1(ptr: const *HANDLE)
{
}

#test
{
    var h: HANDLE
    toto(&h)
    toto1(&h)
}