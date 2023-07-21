..\bin\swag_devmode clean --ignore-bad-params -w:..\bin\examples %1 %2 %3 %4

..\bin\swag_devmode run -w:..\bin\examples -m:wnd    --args:"swag.test" %1 %2 %3 %4
..\bin\swag_devmode run -w:..\bin\examples -m:captme --args:"swag.test" %1 %2 %3 %4
..\bin\swag_devmode run -w:..\bin\examples -m:draw   --args:"swag.test" %1 %2 %3 %4
..\bin\swag_devmode run -w:..\bin\examples -m:img    --args:"swag.test" %1 %2 %3 %4