#[swag.enumflags]
enum Days
{
    None = 0b_0000_0000 // 0
	None1 = 0b_0000_0000
}

var g = @countof(@typeof(Days).name)

#test
{
	//@assert(g == 17)
    }