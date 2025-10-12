const X: [2] s32 = [10, 20]

//#[Swag.Optimize(.All, false)]
//#[Swag.PrintBc]
#test
{
    let ptr = &X[0]
    @assert(ptr[0] == 10)
    @assert(ptr[1] == 20)

    let ptr1 = &X[1]
    @assert(ptr1[0] == 20)
}
