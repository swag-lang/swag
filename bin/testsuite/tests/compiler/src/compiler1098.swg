#[Swag.EnumFlags]
enum Days
{
    None = 0b_0000_0000 // 0
}

struct TT
{
    x:   string
}

var g: TT
func toto() => g

#test
{
    //	@print(toto().x)
    @assert(#typeof(Days).fullname == "__compiler1098.Days")
}
