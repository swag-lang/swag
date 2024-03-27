struct A
{
    r, g: s32
}

#[Swag.Strict]
typealias AL = A

//#[Swag.PrintBc]
//#[Swag.Optim("", false)]
func toto()->AL
{
    return {1, 2}
}

#test
{
    let x = toto()
    @assert(@typeof(x) == AL)
}
