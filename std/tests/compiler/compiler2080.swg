#[swag.printbc]
func convertFloat(value1: any)
{
    kindof := @kindof(value1)
    dataof := @dataof(value1)
    var value: f64
    switch kindof
    {
        case f32: value = cast(f64) :cast(*f32) dataof
        case f64: value = :cast(*f64) dataof
    }
}