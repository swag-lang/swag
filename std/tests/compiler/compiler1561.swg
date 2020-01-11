struct(T) A
{
}

//#[swag.printbc]
#test
{
    var x: A's32
    @assert(@typeof(x).name == "A's32")
}