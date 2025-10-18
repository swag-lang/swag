#global testerror "Err0096"

#[Swag.Overload]
func main() {}
#[Swag.Overload]
func main(b: string) {}

#run
{
    main(1)
}
