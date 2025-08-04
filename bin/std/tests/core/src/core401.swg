using Core

#[Swag.Safety("nan", false)]
#test
{
    @assert(Math.isNaN(Swag.F32.Nan))
    @assert(!Math.isNaN(1.0'f32))
    @assert(Math.isNaN(Swag.F64.Nan))
    @assert(!Math.isNaN(1.0'f64))
}
