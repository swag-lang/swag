//#global testerror

using Swag

//#[Swag.PrintBc]
#test
{
    const x: any = 6
    @assert(cast(s32) x == 6)

    const x1: any = "6"
    @assert(cast(string) x1 == "6")
}
