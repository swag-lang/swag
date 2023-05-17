//#global testerror

alias A = string

#[Swag.Optim("", false)]
#[Swag.Safety("", true)]
#test
{
    a := "toto"
    var x: ref A = &a
    y := x[1..2]
    @assert(y == "ot")
}