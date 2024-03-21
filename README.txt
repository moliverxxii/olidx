Utilitaire de fichiers MIDI System Exclusive (SysEx) pour yamaha DX7 II-D/FD.

Séparer un fichier en plusieurs fichiers:
olidx -u <folder> -f <dx>
<dx> fichier avec des paquets MIDI sysex:
[sysex1]:
[sysex2]:
[sysex3]: Packed32
[sysex4]: 

<folder>:
file1.syx
file2.syx
file3_preset1.syx
file3_preset2.syx
file3_preset3.syx
...
file3_preset32.syx
file4.syx

preset1-32 : noms du preset dans la memoire.

Compacter un dossier en un fichier SysEx:
olidx -p <file> -d <folder>

<folder>:
file1.syx
file2.syx
preset1.syx
preset2.syx
preset3.syx
...
preset_n.syx n<=32
file3.syx

<file>:
[sysex1]
[sysex2]
[sysex_preset1_32]
[sysex3]
