#global testerror "Err0485"

func addItem(name: string, id: string, disabled = false)
{
}

#test
{
    addItem("x", disabled: false)
}