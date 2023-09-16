#global testerror "Err1066"
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