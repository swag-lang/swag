interface I
{
    toto: mtd()
}

struct A
{

}

impl I for A
{
    mtd toto() {}
}

#[Swag.Optim("", false)]
#test
{
    var a: A
    i := cast(I) a

    struct TT
    {
        obj:    *void
        itable: const ^^void
    }

    ptr := cast(*TT) &i
    //@print(&a, " ", ptr.obj)

    t := cast(const *Swag.TypeInfoStruct) ptr.itable[bitcast(uint) -1'int]
    //@print(" ", t.fullname)

    t1 := cast(const *Swag.TypeInfoStruct) ptr.itable[bitcast(uint) -2'int]
    //@print(" ", t1.fullname)

    var itf1: TT
    itf1.obj = ptr.obj
    itf1.itable = acast @itftableof(t, t1)
    @assert(ptr.itable == itf1.itable)
}