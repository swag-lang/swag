//#[Swag.PrintBc, Swag.Optimize(.All, false)]
#test
{
    let x: any = "value"

    switch x // Implicitly checks the type of `x` using `@kindof`.
    {
    case string as str where str == "value":
        @assert(str == "value") // Asserts that `str` is equal to "value"
        break
    }
}
