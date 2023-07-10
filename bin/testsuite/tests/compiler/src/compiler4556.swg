//#global testerror

alias A = string

#[Swag.Optim("", false)]
#[Swag.Safety("", true)]
#test
{
    var a = "toto"
    var x: ref A = &a
    let y = x[1..2]
    @assert(y == "ot")
}