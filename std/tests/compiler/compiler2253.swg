struct A
{
    x = 1
}

//#[swag.printbc]
#test
{
    var res: [1] A
    @assert(res[0].x == 1)
}