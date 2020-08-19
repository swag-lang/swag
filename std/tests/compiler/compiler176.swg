#module "compiler176"
;

#unittest error;
func main ()-> char { return '1'; }
#assert main() / '1';