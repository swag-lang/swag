using Core

#[Swag.Safety("nan", false)]
#test
{
    @assert(Math.isNan(Swag.F32.Nan))
    @assert(!Math.isNan(1.0'f32))
    @assert(Math.isNan(Swag.F64.Nan))
    @assert(!Math.isNan(1.0'f64))
}
