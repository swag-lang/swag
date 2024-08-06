#global testerror "Err0100"
#test
{
    var x = 5
    let y =,constref &x
    y =,ref &x
}