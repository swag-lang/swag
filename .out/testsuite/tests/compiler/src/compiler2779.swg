#[Swag.inline, Swag.discardable]
func atomor(ptr: *u32)->u32 => @atomor(ptr, 0)
func titi(ptr: *u32)
{
    atomor(ptr)
}
