using Core

#test
{
    var h: HashSet'u64
    for 65:
        h.add(#index)

    visit v in h
    {
        @assert(h.contains(v))
    }
}
