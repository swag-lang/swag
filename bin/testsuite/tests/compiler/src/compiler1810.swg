//#[Swag.PrintBc]
#test
{
    let a = "ABCDEFG"
    var c = a[3..6]
    @assert(c == "DEFG")
    @assert(@typeof(c) == @typeof(string))

    c = a[0..1]
    @assert(c == "AB")
}
