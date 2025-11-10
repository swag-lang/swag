#global testerror "Err0556"

func addItem(name: string, id: string, disabled = false) {}

#test
{
    addItem("x", disabled: false)
}
