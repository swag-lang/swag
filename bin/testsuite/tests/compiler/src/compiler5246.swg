#global testerror "Err0093"
#test
{
    var x = 5
    let y = #constref &x
    y = #ref &x
}