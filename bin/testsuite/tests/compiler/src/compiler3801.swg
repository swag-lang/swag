#global testerror "Err0598"

func addItem(name: string, id: string, disabled = false)
{
}

#test
{
    addItem("x", disabled: false)
}