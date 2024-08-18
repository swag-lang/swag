using Core

#test
{
    var x: Array's32
    x.resize(2)

    visit &p, cpt in x
    {
        var v   = #index
        var str = Format.toString("%", #index)
        @assert(str == "0" or str == "1")
    }
}
