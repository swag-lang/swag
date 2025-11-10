//#[Swag.PrintBc, Swag.Optimize(.All, false)]
#test
{
    let x: const [..] s32 = [1, 2]

    let v = func|x|()
    {
        @assert(x[0] == 1)
        @assert(x[1] == 2)
    }

    v()
}
