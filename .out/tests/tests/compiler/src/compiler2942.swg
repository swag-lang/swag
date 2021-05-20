func titi()->u32 => 2331188998

#test
{
    v := titi()
    v1 := v / cast(f32) Swag.U32.Max
    @assert(bitcast(u32) v1 == 1057682207)
}