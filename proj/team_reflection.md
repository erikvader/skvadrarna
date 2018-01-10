# 1 Teamreflektion av Skvadrarna
## 1.1 Deltagarlista
| Namn   | Mejl                           | Aktiv tid |
|------------------|----------------------------|------------------------|
| David Andersson  | d.andersson.1994@gmail.com | 2017-12-11 - 2018-01-11 |
| Daniel Wang      |daniel.zhixi.wang@gmail.com| 2017-12-11 - 2018-01-11 |
| Erik Rimskog     | erik.rimskog@gmail.com                           | 2017-12-11 - 2018-01-11 |
| Hampus Adolfsson | haad5594@student.uu.se | 2017-12-11 - 2018-01-11 |
| Mattias Aronsson Adler | maad9978@student.uu.se | 2017-12-11 - 2018-01-11 |
| Mattias Walla | mattias.walla@gmail.com | 2017-12-11 - 2018-01-11 |
# 2 Kvantifiering
Projektet startade 2017-12-11 och avslutades 2018-01-11. Projektet planerades ha två sprintar, men i slutändan reducerades det till en. Totalt skrevs 1100 rader kod, 75 rader skript kod och 1001 rader tester. Projektet tog 225 timmar och under den tiden gjordes 201 commits, 14 pull requests och 22 Github Issues.

# 3 Processen
## 3.1 Inledning 
Vid projektets start fick Daniel och Hampus till uppgift att läsa på om två olika processer, Scrum och Kanban (andra personer fick annat att läsa om).

Därefter hade vi ett komma igång-möte där vi alla delade med oss om vad vi hade läst. Vi bestämde oss för att vi skulle använda scrum med en del kanban inblandat (vi var ju ända under krav att använda ett kanbanliknande bräde för planering). Vi lade ingen extra tid på att studera scrum efter det, utan körde vidare på det vi redan kunde om det.
## 3.2 Implementation
Vi började med att ha ett planeringsmöte där vi gick igenom specifikationen och diskuterade vad som behövde göras. Vi bestämde att vi efter den första sprinten skulle skapa en heap som kunde hantera pekarlösa objekt, men som inte nödvändigtvis var så snabb eller minneseffektiv. Vi lade även till kort på vårt kanbanbräde för det som var tänkt att göras. Däremot satte vi inte någon deadline eller preliminärt slutdatum för sprinten, vilket förmodligen var ett ganska stort misstag.
Vår handledare gav oss rådet att tillsammans skriva ett gränssnitt i alla .h filer. Vi följde detta råd vilket var till stor hjälp eftersom vi då lätt kunde dela upp arbetet i mindre delar. Uppgiften att implementera .c filerna delades ut till 1-2 person per C fil. Därefter blev det Jullov och alla kunde jobba på egen hand med sina implementationer. 

I samband med det började vi (oavsiktligt) frångå det tänkta scrumspåret. Eftersom vi hade gjort en ‘uppgift’ av varje fil och alla jobbade under olika tider, hände saker i ojämn takt i olika delar av projektet och vi blev lite isolerade i våra egna delar. Utan att vi hade satt ihop något fungerande för att avsluta första sprinten började vi arbeta på andra delar av specen. Istället fortsatte vi mer enligt kanban, där vi löpande satte up kort för sådant som behövdes för att komma vidare. Detta sätt att arbeta på fungerade bra för oss, och gjorde det enkelt att samordna arbetet trots att vi inte kunde träffas fysiskt. Det saknades däremot en mer långtgående planering, såsom vi hade haft med scrum.

Efter nyår var de flesta .c filerna implementerade och därefter har vi byggt ihop, testat och fixat buggar. Då har vi kunnat träffas fysiskt, och alla har haft tid att arbeta mer konsekvent.
# 4 Användande av verktyg
Förutom diverse olika kod editors använde vi ett några gemensamma verktyg. Först användes en funktion i GitHub kallad “Projects”. Detta är en trelloliknande modul direkt kopplat till projektets GitHub-repo där vi lätt och smidigt skapade och följde de olika kanban-korten. Något som var mycket smidigt var att det direkt från denna modul går att skapa en issue i GitHub för kortet och delegera det till en användare. Detta användes för att ge en tydlig överblick om hur projektet gick, vad som arbetades på och vad som behövdes bli gjort.

Vi använde även ett tidloggningsprogram vid namn Tickspot som vi använde för att logga vår tid på kategorierna programmering, testning, möten/planering och rapportskrivning. Tickspot valdes av två anledningar. Först så fanns möjligheten att gratis skapa ett projekt som kunde innehålla sex medlemmar. Sen så har Tickspot även en mobilapp vilket gjorde loggningen av tiden mycket simplare.
För kommunikation användes Slack till en början men då gruppmedlemmarna var mer vana med Discord övergick vi dit.
# 5 Kommunikation, Samarbete och Koordination
Som tidigare nämnts användes Discord för största delen av kommunikationen. Eftersom detta är ett program som alla medlemmarna använder till vardags var det under hela projektet relativt lätt att få tag på varandra. Då medlemmarna även var responsiva på Discord under ledigheten var det under hela projektets gång lätt att kommunicera och diskutera problem och frågor som kom upp under tiden.
Kommunikationen med utomstående skedde via mejl och var extremt låg, endast förekommande vid planering av möte med coachen.

Det kom aldrig fram att någon var omotiverad, stressad eller arg vilket kan ha berott på våran konstanta kommunikation över Discord vilket lärde oss att det är extremt viktig med konsekvent kommunikation mellan gruppmedlemmarna. Även om Discord var väldigt passande för just detta projektet skulle däremot större projekt behöva ett mer projektinriktat kommunikationsverktyg. Förbättringar som skulle vara till hjälp är bland annat smidigare filhantering och bättre nålning av meddelanden.
# 6 Arbetsdelning
Till en början delades projektet upp i programmets olika h-filer vilka fördelades bland medlemmarna, då vissa h-filer innebar betydligt mer arbete än andra fördelades dessa till ett par. När nya problem dök upp gjordes de till issues på GitHub som kunde fångas upp. Eftersom vi lade ner mycket tid i början av projektet på att få fram hur skräpsamlaren skulle se ut och vilka delar som behövdes kunde arbetet delas upp utan att överbelasta någon. Då kommunikationen även var mycket bra fanns även alltid möjligheten att be om hjälp eller fråga om råd vilket även detta hindrade att någons arbetsbörda skulle bli oöverkomligt stor.

För att hålla koll på hur mycket tid medlemmarna lade ner på olika delar av projektet loggades deras tid på kategorierna programmering, testning, möten/planering och rapportskrivning. I figuren nedan sammanfattas den slutgiltliga tidloggningen.

![](https://github.com/IOOPM-UU/skvadrarna/blob/master/proj/pie.png)
# 7 Kvalitetssäkring
Även om mycket tid lades ner på att läsa och förstå instruktionerna ansågs de av gruppen att vara tydliga, vi kan därför vara säkra på att vi fått fram den produkt som efterfrågades. För att försäkra att programmet kvalitet har vi jobbat effektivt med både tester och pull requests. Testerna skrevs under projektets gång av den person som skrivit koden. 

All uppladdning av kod behövde även gå igenom en pull requests där de fick granskas innan de tilläts att sammanfogas med huvudgrenen. Totalt gjordes 14 PRs av sex olika personer med 26 kommentarer. Användandet av PRs ledde ett flertal gånger till att slarvfel hittades, men även att större felaktigheter fångades och rättas. Det tvingade även medlemmarna att sätta sig in i varandras kod. Teamet har under projektets gång jobba med PRs på ett seriöst sätt, nästan alla har haft bra namn som “fixade att allocarna inte nullades” och “Heapmetadata-optimiseringar” och har även innehållit en bra beskrivning av vad som gjorts. Ett exempel på undantag till detta är pull requesten “Privata memes”.
# 8 Reflektion
Vi betygsätter vår process som 3/7, anledningen till det låga betyget är för att vi inte satte oss in så mycket i hur processen fungerade. Vi hade däremot stor hjälp av kanban-brädet vilket inspirerades av processen.
Vi betygsätter vår slutprodukt till 5/7. Produkten innehåller allt från specen förutom en liten lättimplementerad avvikelse, det finns dock några buggar kvar i programmet. Några saker lämnar också rum för förbättring, såsom hur skräpsamling hanteras vid väldigt högt minnestryck.

Vi betygsätter vår kod garanti till 6/7. Vi har genom projektets gång aktivt arbetat med Pull Requests, tester och god kommentering av kod vilket har lett till hög kvalitet av koden.
Vi anser vår största vinst att vara att modulerna när vi slog ihop dem inte gav upphov till stora buggar och vår största förlust är att alla hade olika tentor som alltid gjorde någon otillgängliga.
