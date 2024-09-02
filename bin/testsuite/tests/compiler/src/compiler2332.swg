//#global testerror "Err0289"

#[Swag.Macro]
func toto()
{
    var myVar = 0
    #macro
    {
        alias myVar = #up myVar
        myVar += 1
    }

    @assert(myVar == 1)
}