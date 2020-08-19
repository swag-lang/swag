#module "compiler122"
#unittest pass syntax;

#unittest error;
attr toto() -> ;

#unittest pass syntax;
#unittest error;
attr toto() -> "coucou"