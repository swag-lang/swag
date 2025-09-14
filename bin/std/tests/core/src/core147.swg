using Core, Core.Utf8, Core.Tokenize

//#[Swag.PrintBc]
#test
{
    let x:  String = "指toto指指指titi指tata"
    let arr = x.split('指''rune)
    @assert(arr.count == 3)
    @assert(arr[0] == "toto")
    @assert(arr[1] == "titi")
    @assert(arr[2] == "tata")
}
