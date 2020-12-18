;
;
using swag;

#[constexpr]
func main ()->s32 { return 555; }

var v = main();

#run
{
	@assert(v == 555);
}