#global skip

//#[Swag.PrintBc]
func toto(a, b: s32)->s32
{
    var c = 0

    switch a
    {
    case 0 where b == 0:
        return a + b + 1

    case 0 where b == 1:
        return a + b + 10 

    case 1 where b == 5:
        return 666

    case 1:
        return a * b
        
    default where b == 0:
        return 0
    default where b == 1:
        return 1  
    default:
        return 2      
    }

    return -1
}

#test
{
    @assert(toto(2, 0) == 0)
}