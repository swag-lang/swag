#[Swag.Compiler]
const X: [2] s32 = [10, 20]

#[Swag.Optimize(.All, false), Swag.Compiler]
func test()
{
    let val = X[1]
    @assert(val == 20)
}

#run test()
