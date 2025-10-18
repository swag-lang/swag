#global testerror "Err0724"

func addItem(name: string, id: string, disabled = false) {}

#test
{
    addItem("x", disabled: false)
}
