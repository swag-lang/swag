enum TT
{
    TTA
    TTB
}

func C(value: const& TT)
{
    using TT
    @assert(value == TTA)
}

#test
{
    C(TT.TTA)
}