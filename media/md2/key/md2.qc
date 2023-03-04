/*
A Quake-C file for exporting MD2 files from
MilkShape 3D.
*/

/* Set the origin in the MD2 file compared to the origin in MilkShape 
   This origin refers to the MilkShape coordinate system where z is back-to-
   front, not up/down as in Quark. When in doubt set to 0.0 for all.
*/
$origin 0.0 0.0 0.0	

/* List the skins to be referred to by this model. Write them on each line. 
   The order determines the numbering in 6DX. */
$skin key.pcx

/* List of frame sequences to put in the MD2. In 6DX each will be names
   run01, run02, to run12 and walk01 to walk16. 
   sequence <name> <first frame no> <final frame> 
*/
$sequence static 1 1
