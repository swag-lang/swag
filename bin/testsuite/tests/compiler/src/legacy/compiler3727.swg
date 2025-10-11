//#global testerror
func toto()
{
    var x: any
    if x != null and cast(s32) x == 3:
        @assert(false)
}

#test
{
    toto()
}
