#global testerror "Err0381"

#test
{
    enum Color
    {
        Red
        Green
        Blue
    }

    var a = 0 //Color.Red

    #[Swag.Complete]
    switch a
    {
    }
}
