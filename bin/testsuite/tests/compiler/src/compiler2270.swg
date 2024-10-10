#global testerror "Err0366"

#test
{
    var x = 0
    #[Swag.Complete]
    switch
    {
        case x == 0: x = 50
        case x == 1: @assert(false)
    }
}