###INŠTRUKCIE PRE POUŽÍVATEĽA

Tento program obsahuje štyri .hpp súbory (pathDecomposition, cutSizeBisection,
oneHelpfulSet a redBlackEdges) a dva .cpp súbory - main.cpp ktorý vykonáva
cestovú dekompozíciu grafu, a test.cpp, ktorý súbory .hpp testuje.

###PRED POUŽÍVANÍM PROGRAMU SA NAJPRV UISTITE ŽE SÚ SPLNENÉ TIETO 3 POŽIADVKY:

  1. stiahnite si knižnicu ba-graph ktorú viete nájsť na
    https://bitbucket.org/relatko/ba-graph/commits/67a1f4e718bd4aaf5a389aa54097db227800f27f.
    Pokiaľ nefunguje program správne s najnovšou verziou, skúste si stiahnuť
    verziu z 7.5.2019 ktorá je s týmto projektom kompatibilná.
     - uistite sa že v Makefile je cesta ku knižnici nastavená správne
          - v prvom riadku: CFLAGS - najčastejšie je to ../ba-graph/include/

  2. kompilátor by mal byť aspoň gcc-6 alebo vyšší

###ROZHRANIA

Projekt poskytuje rozhranie pre cestovú dekompozíciu grafu, avšak navyše aj
  ďalšie rozhrania ktoré sú s tým spojené - rez grafu (približná bisekcia), 
  1-pomocná množina a červeno-čierne hrany.
  
- vector<set<Number>> pathDecomposition(Graph &graph);
        - vráti postupnosť množín vrcholov, ktoré patria do grafu graph, pričom spĺňajú podmienky dekompozície
    - vector<set<Number>> pathDecomposition(Graph &graph, int epsilonExp)
        - epsilonExp určuje precíznosť dekompozície (zmysel má hlavne pri väčších vstupoch)
- array<set<Number>, 4> makeBisection(Graph &graph);
        - vráti 4 množiny vrcholov ktoré patria do grafu graph:
          - makeBisection[0] obsahuje vrcholy rezu množiny V0
          - makeBisection[1] obsahuje vrcholy V0
          - makeBisection[2] obsahuje vrcholy rezu množiny V1
          - makeBisection[3] obsahuje vrcholy V1
    - array<set<Number>, 4> makeBisection(Graph &graph, float epsilonExp)
        - epsilonExp určuje precíznosť rezu
- set<Number> getHelpfulSet(Graph &graph, set<Number> v0);
        - vráti podmnožinu množiny vrcholov V0 (resp. V1), ktoré keď presunieme 
          do V1 (V0) zmenšíme rez
        - v graph máme zapamätané hrany - pre túto funkciu veľmi podstatná
    - set<Number> getHelpfulSet(Graph &graph, set<Number> v0, float epsilon)
        - epsilon určuje ako veľká môže daná množina byť
- set<Number> redBlackEdges(Graph &graph, multiset<pair<Number, Number>> &blackEdges);
        - vráti množinu vrcholov grafu graph, pre ktorú platí lema (pre detaily 
          pozri text bakalárskej práce)
        - v multimnožine blackEdges máme uložený zoznam hrán ktoré sú čierne
            - POZOR: záleží na poradí vrcholov! - tak isté, ako boli definované vo vstupe
    - set<Number> redBlackEdges(Graph &graph, multiset<pair<Number, Number>> &blackEdges, float epsilon)
        - podobne ako v getHelpfulSet, epsilon slúži na určenie veľkosti hľadanej
          množiny (bližšie detaily tu nebudeme spomínať)
              
###FORMÁT VSTUPU V main.cpp
                      
  V prvom riadku je číslo určujúce počet vrcholov grafu - pre správne fungovanie musí byť toto číslo párne, inak
  nepôjde o 3-regulárny graf. Pre správne fungovanie programu odporúčame ako vstupné grafy s aspoň 20 vrcholmi.
  Pri menších vstupoch program nemusí fungovať dobre kvôli približnej bisekcii.
  V každom ďalšom riadku bude dvojica čísel - vrcholov v tvare "u v" - hrana medzi vrcholmi u a v.

  Po skončení dekompozície užívateľ môže ukončiť program ("quit") alebo pokračovať (hocičo iné, napríklad "nie").

  Používame Makefile
    - do príkazového riadku napíšte "make" pre kompiláciu a beh programu
    - do príkazového riadku napíšte "make test" pre vytvorenie dlhotrvajúceho testu, následne "test.exe" pre spustenie
    - do príkazového riadku napíšte "make simple_test" pre vytvorenie rýchleho testu, následne "simple_test.exe" pre spustenie

PRAJEM PRÍJEMNE STRÁVENÝ ČAS A DOBRÉ VYSLEDKY S TÝMTO PROJEKTOM :)