//#[swag.printbc]
#run
{
    s := 1
    res := false
    switch s
    {
        case 1:
            fallthrough
        case 2:
            fallthrough            
        case 3:
            res = true
    }
    @assert(res)
}