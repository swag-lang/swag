//#global skip
#test
{
    let a = 1
    var b = 0

    switch a
    {
    case 0 where a == 0:
        break
    case 1 where a == 1:
        b = 1
        break
    }

    @assert(b == 1)
}
