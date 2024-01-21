#global testerror "Err0614"
func(a: bool, x: string) toto() {}
func(a: bool, x: f32) toto() {}

#test
{
    toto'(true, true)()
}