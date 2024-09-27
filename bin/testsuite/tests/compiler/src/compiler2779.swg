#[Swag.Inline, Swag.Discardable]
func atomor(ptr: nn *u32)->u32 => @atomor(ptr, 0)
func titi(ptr: nn *u32)
{
    atomor(ptr)
}
