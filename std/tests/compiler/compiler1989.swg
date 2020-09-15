//#[swag.printbc]
func(T) swap(x, y: *T)
{
    tmp := move nodrop deref x
    deref x = nodrop move nodrop deref y
    deref y = nodrop move nodrop tmp
}

#test
{
    var x: u32 = 666
    var y: u32 = 777
    swap(&x, &y)
    @assert(x == 777)
    @assert(y == 666)
}