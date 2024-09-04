#global testerror "Err0090"
#test
{
    var x = 5
    let y = #constref &x
    y = #constref &x
}