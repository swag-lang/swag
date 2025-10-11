//#[Swag.PrintBc]
#test
{
    var pt     = ["A"]
    let slice: [..] [*] string = [&pt[0]]

    let str = slice[0, 0]
    @assert(str == "A")

    let str1 = slice[0]
    @assert(str1[0] == "A")
}
