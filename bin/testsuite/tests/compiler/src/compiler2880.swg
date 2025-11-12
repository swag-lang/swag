#global testerror "Err0722"

#[Swag.Macro]
func toto(stmt: #code void)
{
    @var alias40 = 0 // swc-expected-error {{parser_err_invalid_intrinsic}}
}
