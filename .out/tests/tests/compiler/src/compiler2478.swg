#[swag.safety("of", true)]
{
#semerror #assert cast(s8) 256 == 0
#semerror #assert cast(s8) 257 == 0
#semerror #assert cast(s64) swag.MaxU64 == 0
#semerror #assert cast(s16) swag.MaxU16 == 0
#semerror #assert cast(s32) swag.MaxU64 == 0
}
