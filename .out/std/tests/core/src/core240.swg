
using core

#test
{
	var x: Array's32
    x.resize(2)

    visit *p, cpt: x
    {
        v := @index
        str := format.toString("%", @index)
        @assert(str == "0" or str == "1")
    }
}