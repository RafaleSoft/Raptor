atom:string=["][ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_0123456789.]*["]
atom:loadimage=loadimage
atom:selectimage=selectimage
atom:end=;

map:string=CString
sequence:load=<loadimage><string><end>
sequence:select=<selectimage><string><end>
alternate:command={load}{select}
sequence:commands={command}{commands}
sequence:main={commands}