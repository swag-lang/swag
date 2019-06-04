#unittest module module195
#unittest pass semantic
using swag;

#[constexpr]
func main: ()->u16
{
	return 45;
}

#assert main as f32 == 45