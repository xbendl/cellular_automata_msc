# cellular_automata_msc
This repository contains a software created for my master thesis "Intruction-controlled cellular automata". Text of my thesis can be found at:
https://www.fit.vut.cz/study/thesis/11121/.en?year=0&stud=bendl (czech language)

=============================================
Diplomova prace: Instrukcemi rizene celularni automaty
Platforma: OS Linux
=============================================

Pozadavky:
==========
1) OS Linux
2) Knihovna Qt v.4

Instalace:
==========
1) Spusteni skriptu ./install.sh s pravy roota (bude se instalovat knihovna Qwt)

Spusteni programu:
==================
Synopsis: ./automaton [-u usage]
-u Nepovinny parametr urcujici vyuziti programu
   1 = evoluce pravidel, 2 = analyza pravidel,
   3 = tvorba instrukce, 4 = tvorba uloh

Struktura programu:
===================
Program je rozdelen pomoci zalozek na tri casti:
- Evoluce instrukci: Slouzi k nalezeni co nejlepsiho reseni zvolene ulohy.
  Reseni je reprezentovano posloupnosti instrukci (optimalizovano pres GA).
- Analyza instrukci: Slouzi k overeni kvality vyvinuteho reseni sledovanim
  chovani automatu v jednotlivych krocich vyvoje.
- Tvorba instrukci: Slouzi k vytvareni instrukci pouzitelnych pri hledani
  reseni vybrane ulohy.
- Tvorba uloh: Slouzi k vytvareni zadani uloh - dvojic pocatecni konfigurace,
  cilova konfigurace pro problem sebe-organizace.

Napoveda v programu:
====================
Ovladaci prvky s nejasnou funkci jsou vybaveny napovedou. K jejimu zobrazeni 
dojde po prejeti mysi nad danym ovladacim prvkem.

Nastaveni parametru:
====================
Cesta k souboru s parametry: ./configuration.txt
- Soubor s vychozimi parametry evoluce a analyzy instrukci vcetne 
  jejich popisu.
- Nekolik nejdulezitejsich parametru lze pozdeji menit ve spustene aplikaci 
  pomoci interaktivnich ovladacich prvku.

POZNAMKY:
=========
- K realizaci grafu v aplikaci zobrazujiciho prubeh evoluce byla vyuzita
  knihovna Qwt (http://qwt.sf.net).
- K praci s XML soubory byla pouzita knihovna TinyXML 
  (http://sourceforge.net/projects/tinyxml/).
- Pro vyzkouseni analyzy instrukci lze vyuzit pripravenych souboru s automaty
  a s resenimi ve slozka "example_tasks" a "example_solutions"


