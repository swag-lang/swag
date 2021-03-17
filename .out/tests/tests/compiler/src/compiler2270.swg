
#global testerror

#test
{
    x := 0
    #[swag.complete]
    switch
    {
        case x == 0: x = 50
        case x == 1: @assert(false)
    }
}