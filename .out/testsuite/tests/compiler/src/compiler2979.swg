#[Swag.compiler]
const X: [2] s32 = @[10, 20]

#[Swag.Optim("", false)]
#[Swag.compiler]
func test()
{
    val := X[1]
    @assert(val == 20)
}

#run test()