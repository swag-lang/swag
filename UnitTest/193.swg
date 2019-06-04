#unittest module module193
#unittest pass semantic

#[swag.constexpr]
func main: ()->s32
{
	return 5;
}

#assert (main as f32) == 5.0
