#global testerror "Err0590"

func addItem(name: string, id: string, disabled = false)
{
}

#test
{
    addItem("x", disabled: false)
}