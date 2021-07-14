//#[Swag.Compiler]
func(T) checker()
{
    //@print(v)
    ptr := @typeof(T)
    @assert(ptr == s32)
}

//#[Swag.PrintBc]
func(T) toto()
{
    //#print "toto"
    #run
    {
        checker'T()
    }
}

//#[Swag.PrintBc]
func titi()
{
    toto's32()
}
