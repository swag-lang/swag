func toto()
{
    #if true
    {
        func titi()
        {
            #if true do
                struct AA {}
            var x: AA
        }
    }

    titi()
}

#test
{
    toto()
}
