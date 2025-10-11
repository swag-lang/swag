#test
{
    var x = 66
    var y = #ref &x
    @init(y)
    @assert(x == 0)
    @init(y)(20)
    @assert(x == 20)
}
