//#global skip
enum TOTO: const [..] s32
{
    R = [1, 2]
    G = [3, 4, 5]
    B = [0]
}

#[Swag.Optimize(.All, false)]
//#[Swag.PrintBc]
#test
{
    using Swag

    let t   = #typeof(TOTO)
    let ptr = cast(const *TypeInfoEnum) t
    @assert(@countof(ptr.values) == 3)

    @assert(ptr.values[0].name == "R")
    @assert(ptr.values[1].name == "G")

    let ptr1 = ptr.values[1]
    @assert(ptr1.name == "G")

    let ptrVal = cast(const [..] s32) ptr1.value
    @assert(ptrVal[0] == 3)
}
