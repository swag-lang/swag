
using Core

#test
{
    //try jobsystem.init(4)
    var arr: Array'String
    arr.resize(10)
    loop 10
    {
        @assert(dref arr[@index] == "")
        arr[@index] = String.from("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX")
    }

    arr.resize(5)
    loop 5
    {
        @assert(dref arr[@index] == "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX")
    }
}