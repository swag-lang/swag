#global testerror "Saf0001"
using Swag

#[Swag.Safety("", true)]
#[Swag.Sanity(false)]
#test
{
    var x: any = 6
    let a = cast(string) x
    @assert(a == "0")
    x = 7
}
