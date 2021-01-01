struct AA
{
    s = "toto"
}

//#[swag.printbc]
#test
{
    var arr: [4] AA
    visit v: arr
    {
        @assert(v.s == "toto")
    }

    var arr1: [4, 4] AA
    visit v: arr1
    {
        @assert(v.s == "toto")
    }
}