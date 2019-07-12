###IN�TRUKCIE PRE POU��VATE�A

Tento program obsahuje �tyri .hpp s�bory (pathDecomposition, cutSizeBisection,
oneHelpfulSet a redBlackEdges) a jeden .cpp s�bor - main.cpp ktor� umo��uje
vybra� �i bude vykonan� cestov� dekompoz�cia, rez grafu, n�jdenie 1-pomocnej
mno�iny alebo mno�iny v �erveno-�iernom grafe.

###PRED POU�9VAN�M PROGRAMU SA NAJPRV UJISTITE �E S� SPLNEN� TIETO 3 PO�IADVKY

  1. stiahnite si kni�nicu ba-graph ktor� viete n�js� na 
    https://bitbucket.org/relatko/ba-graph/commits/67a1f4e718bd4aaf5a389aa54097db227800f27f.
    Pokia� nefunguje program spr�vne s najnov�iou verzoiu, sk�ste si stiahnu�
    verziu z 7.5.2019 ktor� je s t�mto projektom kompatibiln�.

  2. kompil�tor by mal by� aspo� gcc-6 alebo vy���
  
  3. v pr�pade pou�itia grafov zo s�borov .txt upravte v main.cpp absol�tnu
    cestu pre dosiahnutie prie�inku graph_examples

###ROZHRANIA

Projekt poskytuje rozhranie pre cestov� dekompoz�ciu grafu, av�ak navy�e aj
  �al�ie ktor� s� s t�m spojen� - rez grafu (pribli�n� bisekcia), 1-pomocn�
  mno�ina a �erveno-�ierne hrany.
  
- vector<set<Number>> pathDecomposition(Graph &graph, Factory &f);
        - vr�ti postupnos� mno��n vrcholov, ktor� patria do grafu graph vo
          factory f, pri�om sp��aj� podmienky dekompoz�cie
- pair<pair<set<Number>, set<Number>>, pair<set<Number>, set<Number>>> 
    makeBisection(Graph &graph, Factory &f);
        - vr�ti 4 mno�iny vrcholov ktor� patria do grafu graph:
          - .first.first obsahuje vrcholy rezu mno�iny V0
          - .first.second obsahuje vrcholy V0
          - second.first obsahuje vrcholy rezu mno�iny V1
          - second.second obsahuje vrcholy V1
- set<Number> getHelpfulSet
    (Graph &graph, set<Number> v0, float epsilon, Factory &f);
        - vr�ti podmno�inu mno�iny vrcholov V0 (resp. V1), ktor� ke� presunieme 
          do V1 (V0) zmen��me rez
        - v graph m�me zapam�tan� hrany - pre t�to funkciu ve�mi podstatn�
        - epsilon ur�uje ako ve�k� m��e dan� mno�ina by�
- set<Number> redBlackEdges(Graph &graph, multiset<pair<Number, Number>> 
    &blackEdges, float epsilon, Graph &blackGraph, Factory &f);
        - vr�ti mno�inu vrcholov grafu graph, pre ktor� plat� lema (pre detaily 
          pozri text bakal�rskej pr�ce)
        - v multimno�ine blackEdges m�me ulo�en� zoznam hr�n ktor� s� �ierne
            - POZOR: z�le�� na porad� vrcholov!
        - podobne ako v getHelpfulSet sl��i epsilon na ur�enie ve�kosti h�adanej
          mno�iny (bli��ie detaily tu nebudeme spom�na�)
        - blackGraph predstavuje graf generovan� �iernymi hranami z blackEdges
            - t�to premenn� vyu��vame u� vo funkcii getHelpfulSet, tak�e hoci je
              popri premenn�ch graph a blackEdges redundantn� ke�e ju vieme
              nimi vygenerova�, namiesto op�tovn�ho generovania si ju ulo��me
              
###AKO BY MAL VYZERA� VSTUP PROGRAMU
                      
  Graf m��e by� vlo�en� 3 sp�sobmi: (rie�en� v main.cpp)
    1. "g" = vygenerova� n�hodn� regul�rny multigraf (funkcia z ba-graph).
      N�sledne treba vybra� po�et vrcholov. Graf by mal by� kubick�, �i�e ka�d�
      sused bude ma� 3 susedov. T�m p�dom, po�et vrcholov mus� by� p�rny.                    
        1.1. po vygenerovan� m�te mo�nos� zisti� �o za graf ste vygenerovali "y"
        1.2. pokia� o to nem�te z�ujem, nap�te "n" (alebo hoci�o in�)
    2. "f" = vlo�i� graf z .txt s�boru. S�bor by mal by� v prie�inku 
      graph_examples. Form�t s�boru mus� by� v tvare: prv� riadok obsahuje
      po�et vrcholov, ka�d� �al�� riadok obsahuje pr�ve jednu hranu (dvojicu 
      vrcholov) - ujistite sa �e graf je kubick�, inak program m��e zlyha�.
        PRIPOMIENKA: nezabudnite zmeni� absol�tnu cestu
    3. "c" = graf z pr�kazov�ho riadka. Form�t a po�iadavky s� rovnak� ako v 
    predch�dzaj�com pr�pade.
    
  Po nastaven� vstupn�ho grafu m�te mo�nos� vybra� �i sa vykon� rez grafu ("c"),
    cestov� dekompoz�cia ("d"),  n�jdenie 1-pomocnej mno�iny ("o") alebo mno�inu
    vrcholov v �erveno-�iernom grafe("r"). Pokia� stla��te hoci�o in�, 
    automaticky sa vykon� dekompoz�cia.
  
  V pr�pade stla�enia "o" treba navy�e nastavi� jednu z mno��n rezu.
  V pr�pade stla�enia "r" treba navy�e ur�i� ktor� hrany s� �ierne a ktor�
    �erven�. V prvom riadku treba nap�sa� po�et �iernych hr�n, n�sledne v ka�dom
    �a��om riadku vyp�sa� jednu hranu - dvojicu vrcholov. 
    UPOZORNENIE: Vrcholy musia by� v porad� v akom boli definovan�.
    
  Vo v�etk�ch algoritmoch je mo�n� nastavi� h�bku(prec�znos�). 
    V prv�ch dvoch pr�padoch (d/c) na stupnici od 1 (povrchov�, r�chle) po 15 
    (h�bkov�, pomal�). V druh�ch dvoch pr�padoch (o/r) treba nastavi� kladn�
    hodnotu re�lnemu ��slu epsilon tak, aby n�sledn� nerovnica platila.
      - "o": (1/3 + 2*epsilon)*|V0| < ve�kos�Rezu
      - "r": |R| > (1/2 + epsilon)*|V|
    
    A. V pr�pade dekompoz�cie je ako v�stup na prvom riadku vyp�san� cestov�
      ��rka a n�sledne zoznam vriec, t.j. samotn� cestov� dekompoz�cia.
      Po jej vyp�san� m� u��vate� mo�nos� spr�vnos� dekompoz�cie overi�.
      
    B. V pr�pade rezu grafu je ako v�stup vyp�san� ��rka rezu, rezov� vrcholy
      (nesk�r s� pou�it� ako parameter pre cestov� dekompoz�ciu) a samotn�
      mno�iny A a B s vrcholmi patriacimi do nich.
      
    C. V pr�pade 1-pomocnej mno�iny sa na v�stupe vyp�e mno�ina vrcholov.
      (teoreticky aj pr�zdna mno�ina, pokia� nepr�zdna neexistuje)
    
    D. V pr�pade �erveno-�iernych hr�n sa na v�stupe vyp�e mno�ina vrcholov. 
      (teoreticky aj pr�zdna mno�ina, pokia� nepr�zdna neexistuje)
      
  Po skon�en� dekompoz�cie, rezu grafu, 1-pomocnej mno�iny �i �erveno-�iernych
    hr�n m� u��vate� mo�nos� skon�i� ("quit") alebo pokra�ova� (hoci�o in�, 
    napr�klad "nie"). Ak zvol� mo�nos� pokra�ova�, m� mo�nos� pracova� s grafom 
    ktor� bol pou�it� naposledy alebo vstupn� graf zmeni� (y/n)
    
PRAJEM PR�JEMNE STR�VEN� �AS A DOBR� V�SLEDKY S T�MTO PROJEKTOM :)