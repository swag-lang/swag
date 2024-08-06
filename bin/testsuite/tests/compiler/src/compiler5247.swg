#global testerror "Err0097"
#test
{
    var x = 5
    let y =,constref &x
    y =,constref &x
}