enum TOTO: string
{
    R = "str100"
    G = "str200"
    B = "str300"
}

//#[Swag.PrintBc]
#[Swag.Warning("Wrn0003", .Disable)]
#test
{
    using Swag

    let t   = #typeof(TOTO)
    let ptr = cast(const *TypeInfoEnum) t
    @assert(@countof(ptr.values) == 3)

    @assert(ptr.values[0].name == "R")
    @assert(ptr.values[1].name == "G")

    var ptr1 = ptr.values[1]
    @assert(ptr1.name == "G")

    let ptrVal = cast(const *string) ptr1.value
    @assert(dref ptrVal == "str200")
}
