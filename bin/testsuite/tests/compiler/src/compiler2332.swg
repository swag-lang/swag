#global testerror "Err0394"
#test
{
    var myVar = 0
    #macro
    {
        typealias myVar = #up myVar
        myVar += 1
    }

    @assert(myVar == 1)
}