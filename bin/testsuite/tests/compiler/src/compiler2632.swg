func toto()
{
    #if true
    {
        func titi()
        {
            #if true struct AA {}
            var x: AA
        }
    }

    titi()
}

#test
{
    toto()
}