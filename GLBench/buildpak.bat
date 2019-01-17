ECHO Making GLBench data package  ...

cd "%RAPTOR_ROOT%\GLBench\Datas"

"%RAPTOR_ROOT%\Redist\Bin\RaptorDataPackager.exe" -C GLBench.pck M1_1.jpg M1_1024.jpg M1_128.jpg M1_1280.jpg M1_16.jpg M1_2.jpg M1_256.jpg M1_32.jpg M1_4.jpg M1_512.jpg M1_64.jpg M1_8.jpg M74_1024.jpg M74_256.jpg SmallTeapot.3DS

DEL /F "*.zip"

cd ..

IF EXIST GLBench.pck (
	ECHO GLBench package already exist, it will be replaced !
	DEL /F GLBench.pck
)

move Datas\GLBench.pck .

:END

