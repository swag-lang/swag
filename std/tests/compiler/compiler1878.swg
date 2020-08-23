/*#[swag.inline]
func clamp(x, low, up: f32) => x < low ? low : (x > up ? up : x)

#[swag.printbc]
#test
{
    x := 2.0'f32

    if(x < 0)
    {
        @print("lo")
        x = 0;
    }
    else if(x > 1)
    {
        @print("gr")
        x = 1;
    }
    else
    {
        @print("??")
    }

    //x := clamp(2.0'f32, 0, 1)
    @print(cast(f64) x)
}*/