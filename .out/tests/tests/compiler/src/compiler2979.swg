#[Swag.compiler]
const X: [2] s32 = @[10, 20]

#[Swag.optim("", false)]
#[Swag.compiler]
func test()
{
    val := X[1]
    @print(val)
}

#run test()