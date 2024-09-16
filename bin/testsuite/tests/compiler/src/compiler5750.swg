//#[Swag.PrintBc, Swag.Optimize("", false)]
#test
{
    let x: any = "value"

    switch x    // Implicitly checks the type of `x` using `@kindof`.
    {
    case let str as string where str == "value":       // Matches if `x` is a string and equals "value"
        @assert(str == "value")                     // Asserts that `str` is equal to "value"
        break   
    }
}