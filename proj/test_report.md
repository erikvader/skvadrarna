# Tester
Projektets tester befinner sig i mappen `test/` och kan köras med `make test`, eller `make memtest` för att köra med valgrind. Alla tester använder CUnit, förutom testerna för objektmetadatan, som använder ett hemsnickrat ramverk. Testerna är uppdelade i filer efter den källkodsfil de testar. Alla funktioner är enhetstestade utom de i [gc_alloc.c](../src/gc_alloc.c). 

För integrationstester används oss av en botten-upp-strategi, där varje modul testas individuellt, därefter testas moduler som beror på dem modulerna, o.s.v. Interaktioner mellan moduler testas på så sätt genom att enhetstesta integrerande moduler. För interaktionen mellan heap-metadata och objekt-metadata finns även dedikerade integrationstester (som inkluderas i `make test`).

Regressionstestning har framförallt gjorts genom att kontinuerligt använda och uppdatera enhetstester. Enhetstester för nästan alla funktioner byggdes upp tidigt i projektets skede, och allt eftersom funktionalitet har byggts på har dessa använts för att se till att inget går sönder i processen. Samtidigt har testerna utökats för att även säkra den nya funktionaliteten. I den mån buggar har hittats som inte upptäcks av enhetstesterna har nya tester lagts till som visar på att felet är är löst (och inte har återuppstått).

Tack vare bra testning verkar de flesta fel ha kunnat åtgärdas direkt när de skrevs. Bara ett fåtal riktiga buggar som inte täcks av enhetstester har hittats (genom kodgranskning och mer övergripande tester). Därför är det också få buggar som har ägnats en issue på github.

**Varning**: Läskiga buggar

https://github.com/IOOPM-UU/skvadrarna/issues/29

https://github.com/IOOPM-UU/skvadrarna/issues/14

https://github.com/IOOPM-UU/skvadrarna/issues/32

https://github.com/IOOPM-UU/skvadrarna/issues/25

https://github.com/IOOPM-UU/skvadrarna/issues/34

https://github.com/IOOPM-UU/skvadrarna/issues/35

