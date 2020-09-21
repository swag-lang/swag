//#[swag.printbc]
func(T) swap(x, y: *T)
{
    tmp := move nodrop :x
    :x = nodrop move nodrop :y
    :y = nodrop move nodrop tmp
}

#test
{
    var x: u32 = 666
    var y: u32 = 777
    swap(&x, &y)
    @assert(x == 777)
    @assert(y == 666)
}