using Core

#test
{
    var list: List's32
    n := list.addFront(1)
    list.addBack(3)
    list.insertBefore(n, 0)
    list.insertAfter(n, 2)

    cpt := 0
    val := 0
    visit v: list
    {
        cpt += 1
        val += v
    }

    @assert(cpt == 4)
    @assert(val == 6)

    visit v: list
    {
        cpt += 1
        break
    }

    @assert(cpt == 5)
}