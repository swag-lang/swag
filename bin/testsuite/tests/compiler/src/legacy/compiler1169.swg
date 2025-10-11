#global testerror "Err0040"

var TOTO = "win32"

#if TOTO == "linux"
{
    func toto()
    {
        @assert(false)
    }
}
#else
{
    func toto() {}
}

#test
{
    toto()
}
