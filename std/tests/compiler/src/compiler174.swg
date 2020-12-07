;
;
using swag;

#[constexpr]
func main () -> s32
{
	return -1;
}

#assert(-main() == 1);
#assert((4 * 5) * main() == -20);

#test
{
	@assert((4 * 5) * main() == -20);
}
