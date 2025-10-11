//#global skip

enum TOTO
{
    R = 100
    G = 200
    B = 300
}

//#[Swag.PrintBc]
#test
{
    using Swag

    var t = #typeof(TOTO)
    t = #typeof(TOTO)
    let ptr = cast(const *TypeInfoEnum) t
    @assert(@countof(ptr.values) == 3)

    let ptr1 = ptr.values[1]
    @assert(ptr1.name == "G")

    let ptrVal = cast(const *s32) ptr1.value
    @assert(dref ptrVal == 200)
}
