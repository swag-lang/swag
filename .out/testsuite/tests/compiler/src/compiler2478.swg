#[Swag.Safety("overflow", true)]
{
#semerror #assert cast(s8) 256 == 0
#semerror #assert cast(s8) 257 == 0
#semerror #assert cast(s64) Swag.U64.Max == 0
#semerror #assert cast(s16) Swag.U16.Max == 0
#semerror #assert cast(s32) Swag.U64.Max == 0
}
