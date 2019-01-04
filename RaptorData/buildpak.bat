ECHO Making Raptor data package  ...

cd "%RAPTOR_ROOT%\RaptorData\Data"

"%RAPTOR_ROOT%\Redist\Bin\RaptorDataPackager.exe" -C RaptorData.pck Raptor_logo_sml.txt rapsplsh.AVI lucon.ttf RaptorMessages.xml bump_0light.vp bump_0light.fp bump.vp bump.fp bump_att.vp bump_att.fp bump_att_2light.vp bump_att_2light.fp bump_att_3light.vp bump_att_3light.fp embm_0light.vp embm_0light.fp embm.vp embm.fp blinn.vs blinn.ps phong.vs phong.ps bump.vs bump.ps projection.fp shadowmap.fp shadowmap_pcf.fp shadowmap_pcf_4x.fp shadowmap_pcf_16x.fp blenderX_8x.vp blenderY_8x.vp blender_8x.fp AO.vs AO.ps

DEL /F "*.zip"

cd ..

IF EXIST RaptorData.pck (
	ECHO RaptorData package already exist, it will be replaced !
	DEL /F RaptorData.pck
)

move Data\RaptorData.pck .

:END

