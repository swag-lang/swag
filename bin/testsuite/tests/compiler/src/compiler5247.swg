#global testerror "Err0104"
#test
{
    var x = 5
    let y =,constref &x
    y =,constref &x
}