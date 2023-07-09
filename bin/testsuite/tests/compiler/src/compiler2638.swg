namespace AA
{
    struct ST
    {

    }
}

namespace BB
{
    struct ST
    {

    }
}

func(T) toto()
{

}

#test
{
    var ptr1 = @typeof(toto'(AA.ST))
    var ptr2 = @typeof(toto'(BB.ST))
    @assert(ptr1 != ptr2)
}