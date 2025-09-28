using Swag

//#[Swag.PrintBc]
#test
{
    let ptr = #typeof(const *TypeInfo)
    //@print(ptr.name)
    @assert(ptr.fullname == "const *Swag.TypeInfo")
}
