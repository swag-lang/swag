#global testerror "Err0592"
#[Swag.Overload]
func(a: bool, x: string) toto() {}
#[Swag.Overload]
func(a: bool, x: f32) toto() {}

#test
{
    toto'(true, true)()
}
