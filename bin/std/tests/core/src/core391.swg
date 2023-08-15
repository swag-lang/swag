using Core

//#[Swag.PrintBc]
#test
{
    var arr: Array'String
    arr.add(String.from("a"))
    arr.add(String.from("a"))
    arr.add(String.from("a"))

    visit s: arr
    {
        //@print(@typeof(s).name, " ")
        @assert(s[0] == `a`'u8)
    }
}