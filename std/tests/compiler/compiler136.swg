#module "compiler136"
#unittest pass syntax;

#unittest error;
attr toto(b: ) -> var;
#unittest error;
attr toto(b, ) -> var;
