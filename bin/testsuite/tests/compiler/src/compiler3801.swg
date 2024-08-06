#global testerror "Err0548"

func addItem(name: string, id: string, disabled = false)
{
}

#test
{
    addItem("x", disabled: false)
}