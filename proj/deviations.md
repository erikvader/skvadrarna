# Deviations
Skräpsamlaren har allt den ska förutom en punkt:
* Formatsträngen till h_alloc_struct kan inte innehålla siffror. t.ex. ska “3c” vara detsamma som “ccc” men så är inte fallet.
Anledningen till att detta inte implementerats är för att det missades när designen gjordes och när felet hittades fanns inte tid att göra några utökningar till programmet. Det skulle lätt kunna implementeras genom att utveckla switchen i funktionen byte_array64 i object_metadata.c att även ha cases för siffror.
