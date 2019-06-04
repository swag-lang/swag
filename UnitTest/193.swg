#unittest module module193
#unittest pass semantic

#[swag.constexpr]
func main: ()->s8
{
	return 5;
}

#assert (main as f32) == 5.0
