namespace AA
{
    struct ST {}
}

namespace BB
{
    struct ST {}
}

func(T) toto() {}

#test
{
    let ptr1 = #typeof(toto'(AA.ST))
    let ptr2 = #typeof(toto'(BB.ST))
    @assert(ptr1 != ptr2)
}
