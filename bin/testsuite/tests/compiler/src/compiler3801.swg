#global testerror "Err0593"

func addItem(name: string, id: string, disabled = false)
{
}

#test
{
    addItem("x", disabled: false)
}