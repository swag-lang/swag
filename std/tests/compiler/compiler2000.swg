//#[swag.printbc]
#[swag.inline]
func swap(x, y: *s32)
{
    tmp := :x
    :x = :y
    :y = tmp
}

//#[swag.printbc]
#[swag.nooptim]
#test
{
    xxx := 5
    yyy := 10
    swap(&xxx, &yyy)
    @assert(xxx == 10)
    @assert(yyy == 5)
}