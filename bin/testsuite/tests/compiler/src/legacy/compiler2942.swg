func titi()->u32 => 2331188998

#test
{
    let v  = titi()
    let v1 = v / cast(f32) Swag.U32.Max
    @assert(cast #bit (u32) v1 == 1057682207)
}
