#global testerror "Err0356"

#test
{
    let a = 0
    let b = 0
    var c = 0

    switch
    {
    case a == 0 where b == 0:
        break
    case 0 where b == 1:
        c = 1
        break        
    }

    @assert(b == 1)
}
