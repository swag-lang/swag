using Core

#test
{
    var h: HashSet'u64
    loop 65 do
        h.add(#index)

    visit v in h
    {
        @assert(h.contains(v))
    }
}
