using Core

#test
{
    //try jobsystem.init(4)
    var arr: Array'String
    arr.resize(10)
    for 10
    {
        @assert(arr[@index] == "")
        arr[@index] = String.from("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX")
    }

    arr.resize(5)
    for 5
    {
        @assert(arr[@index] == "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX")
    }
}
