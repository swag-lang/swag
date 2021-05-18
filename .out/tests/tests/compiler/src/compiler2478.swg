#[swag.safety("overflow", true)]
{
#semerror #assert cast(s8) 256 == 0
#semerror #assert cast(s8) 257 == 0
#semerror #assert cast(s64) swag.U64.Max == 0
#semerror #assert cast(s16) swag.U16.Max == 0
#semerror #assert cast(s32) swag.U64.Max == 0
}
