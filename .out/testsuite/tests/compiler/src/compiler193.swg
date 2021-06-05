;
;

#[Swag.constexpr]
func main ()->s32
{
	return 5;
}

#assert cast(f32) main() == 5.0;
