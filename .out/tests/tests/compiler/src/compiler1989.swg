//#[swag.printbc]
func(T) swap(x, y: *T)
{
    tmp :=,moveraw :x
    :x =,nodrop,moveraw :y
    :y =,nodrop,moveraw tmp
}

#test
{
    var x: u32 = 666
    var y: u32 = 777
    swap(&x, &y)
    @assert(x == 777)
    @assert(y == 666)
}