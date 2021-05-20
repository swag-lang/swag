//#[Swag.compiler]
func(T) checker()
{
    //@print(v)
    ptr := @typeof(T)
    @assert(ptr == s32)
}

//#[Swag.printbc]
func(T) toto()
{
    //#print "toto"
    #run
    {
        checker'T()
    }
}

//#[Swag.printbc]
func titi()
{
    toto's32()
}
