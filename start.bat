if Exist "build" (
	cd build
	Start "app" app.exe
	injector.exe app.exe lib.dll
)
