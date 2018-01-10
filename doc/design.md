# Design
I detta dokument hittar du en genomgång av hur skräpsamlaren är uppbyggd och implementerad. Du som läsare antas ha läst [projektets specifikation](http://wrigstad.com/ioopm/2017/project2.html), och delar som redan finns i specifikationen hålls därför kortfattade.
## Överblick
Biblioteket är uppdelat i sex moduler med olika ansvarsområden.
### Huvudmodul
Huvudmodulen ansvarar för att koppla samman alla andra delar av programmet, men implementerar i övrigt inte särskilt mycket funktionalitet.
### Heapens metadata
Denna modul hanterar heapens metadata och ansvarar på så sätt för att hantera hela heapens minne. Här finns funktionalitet för att reservera och avreservera minne på heapen, beräkna kvarvarande minne etc. Alla bestående variabler som behövs av andra moduler (t.ex. Tröskelvärdet för skräpsamling) lagras av den här modulen.
### Objektmetadata
Ansvarar för att hantera varje enskilt objekt. Modulen ser till att objekt på heapen allokeras med relevant information, och tillhandahåller funktioner för att läsa och modifiera den informationen.
### Allokering
Allokeringsmodulen för samman objektmetadatamodulen och heapmetadatamodulen, och ansvarar för korrekt allokering av objekt (med metadata) på heapen.
### Stackiterator
Den här modulen är till för att iterera genom potentiella pekare på stacken.
### GC-event
Ansvarar för att utföra själva skräpsamlingen.
Interaktionen mellan dessa moduler sammanfattas i figuren nedan:

![](https://github.com/IOOPM-UU/skvadrarna/blob/master/doc/figur1.png)

Där en pil från A till B representerar att A är beroende av B. Observera att interaktionerna för tydlighetens skull är något förenklade.
Som kan utläsas binder huvudmodulen ihop funktionalitet kring initiering, allokering och skräpsamling. Allokeringsmodulen använder sig av heapmetadatan för att reservera minne, och objektmetadatan för att spara information om allokerade objekt. GC-eventmodulen kan sedan använda datan sparad av respektive metadatamodul för att få den information som behövs för att utföra skräprensningen. Metadatamodulerna kan på så sätt ses som passiva moduler genom vilka långvarig data kan skrivas och läsas, medan allokering- och GC-modulerna (och till viss del huvudmodulen) är aktiva moduler som agerar på metadatan.
## Initiering
Initiering sker genom att huvudmodulen allokerar ett större minnesutrymme på programmets heap, som sedan blir en heap som kan användas av programmet. Minnesutrymmet initieras sedan av heapmetadatamodulen, där relevanta variabler och allokeringsdata skrivs till heapens början. För att underlätta skräpsamling delas minnet upp i lika stora chunks, som alla kan hanteras oberoende av varandra.
## Allokering
Allokering sker i huvudsak i allokeringsmodulen. Heapmetadatamodulen anropas för att reservera minne, och samtidigt reserveras lite extra minne för objektets metadata. Sedan anropas objektmetadatamodulen för att skriva objektdata till början av det reserverade minnet. Objektmetadatan innehåller information om objektets storlek samt om, och i så fall var, objektet innehåller pekare.
Efter varje allokeringsanrop kontrollerar huvudmodulen minnestrycket, och kör en skräpsamling om det är för högt.
## Skräpsamling
Skräpsamlingen görs genom att med hjälp av stackiteratormodulen gå igenom alla giltiga pekare på stacken. En pekares giltighet bedöms med hjälp av allokeringsdatan i heapens metadata.

Om stackpekare ska räknas som osäkra gås hela stacken först igenom en gång för att ta reda på vilka chunks som har direkta stackpekare till sig, och därför inte får flyttas. Sedan nollställs iteratorn för att gå igenom alla pekare igen. För varje utpekat objekt följs även det objektets pekare upp rekursivt, för att nå alla levande objekt. Varje upphittat objekt som inte ligger i en osäker chunk flyttas till en annan, oanvänd chunk, och markeras som utforskat (för att hantera cykliska strukturer). Sist rensas (avreserveras) alla chunks som tidigare innehöll något och inte räknades som osäkra.
