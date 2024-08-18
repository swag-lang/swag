using Core

#test
{
    var list: List's32
    let n     = list.addFront(1)
    list.addBack(3)
    list.insertBefore(n, 0)
    list.insertAfter(n, 2)

    var cpt = 0
    var val = 0
    visit v in list
    {
        cpt += 1
        val += v
    }

    @assert(cpt == 4)
    @assert(val == 6)

    visit v in list
    {
        cpt += 1
        break
    }

    @assert(cpt == 5)

    cpt = 0
    visit #back v in list
    {
        cpt += 1
        continue
    }

    @assert(cpt == 4)
}
