#global testerror

alias A = string

#[Swag.Optim("", false)]
#[Swag.Safety("", true)]
#test
{
    var x: A
    y := x[1..2]
}