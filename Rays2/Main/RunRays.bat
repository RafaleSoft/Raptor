set RAYS_PATH=F:\Rays2
set RAYS_IP=192.168.0.103

start "Rays Deamon" /B /D %RAYS_PATH%\RaysDeamon RaysDeamon.exe -ip %RAYS_IP%

start "RaysServer" /B /D %RAYS_PATH%\RaysServer RaysServer.exe  -ip %RAYS_IP%

start "RaysClient" /B /D %RAYS_PATH%\MiniRaysClient MiniRaysClient.exe -ip %RAYS_IP%