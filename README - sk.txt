###INŠTRUKCIE PRE POUÍVATE¼A

Tento program obsahuje štyri .hpp súbory (pathDecomposition, cutSizeBisection,
oneHelpfulSet a redBlackEdges) a jeden .cpp súbor - main.cpp ktorı umoòuje
vybra èi bude vykonaná cestová dekompozícia, rez grafu, nájdenie 1-pomocnej
mnoiny alebo mnoiny v èerveno-èiernom grafe.

###PRED POU9VANÍM PROGRAMU SA NAJPRV UJISTITE E SÚ SPLNENÉ TIETO 3 POIADVKY

  1. stiahnite si kninicu ba-graph ktorú viete nájs na 
    https://bitbucket.org/relatko/ba-graph/commits/67a1f4e718bd4aaf5a389aa54097db227800f27f.
    Pokia¾ nefunguje program správne s najnovšiou verzoiu, skúste si stiahnu
    verziu z 7.5.2019 ktorá je s tımto projektom kompatibilná.

  2. kompilátor by mal by aspoò gcc-6 alebo vyšší
  
  3. v prípade pouitia grafov zo súborov .txt upravte v main.cpp absolútnu
    cestu pre dosiahnutie prieèinku graph_examples

###ROZHRANIA

Projekt poskytuje rozhranie pre cestovú dekompozíciu grafu, avšak navyše aj
  ïalšie ktoré sú s tım spojené - rez grafu (pribliná bisekcia), 1-pomocná
  mnoina a èerveno-èierne hrany.
  
- vector<set<Number>> pathDecomposition(Graph &graph, Factory &f);
        - vráti postupnos mnoín vrcholov, ktoré patria do grafu graph vo
          factory f, prièom spåòajú podmienky dekompozície
- pair<pair<set<Number>, set<Number>>, pair<set<Number>, set<Number>>> 
    makeBisection(Graph &graph, Factory &f);
        - vráti 4 mnoiny vrcholov ktoré patria do grafu graph:
          - .first.first obsahuje vrcholy rezu mnoiny V0
          - .first.second obsahuje vrcholy V0
          - second.first obsahuje vrcholy rezu mnoiny V1
          - second.second obsahuje vrcholy V1
- set<Number> getHelpfulSet
    (Graph &graph, set<Number> v0, float epsilon, Factory &f);
        - vráti podmnoinu mnoiny vrcholov V0 (resp. V1), ktorú keï presunieme 
          do V1 (V0) zmenšíme rez
        - v graph máme zapamätané hrany - pre túto funkciu ve¾mi podstatné
        - epsilon urèuje ako ve¾ká môe daná mnoina by
- set<Number> redBlackEdges(Graph &graph, multiset<pair<Number, Number>> 
    &blackEdges, float epsilon, Graph &blackGraph, Factory &f);
        - vráti mnoinu vrcholov grafu graph, pre ktorú platí lema (pre detaily 
          pozri text bakalárskej práce)
        - v multimnoine blackEdges máme uloenı zoznam hrán ktoré sú èierne
            - POZOR: záleí na poradí vrcholov!
        - podobne ako v getHelpfulSet slúi epsilon na urèenie ve¾kosti h¾adanej
          mnoiny (blišie detaily tu nebudeme spomína)
        - blackGraph predstavuje graf generovanı èiernymi hranami z blackEdges
            - túto premennú vyuívame u vo funkcii getHelpfulSet, take hoci je
              popri premennıch graph a blackEdges redundantná keïe ju vieme
              nimi vygenerova, namiesto opätovného generovania si ju uloíme
              
###AKO BY MAL VYZERA VSTUP PROGRAMU
                      
  Graf môe by vloenı 3 spôsobmi: (riešené v main.cpp)
    1. "g" = vygenerova náhodnı regulárny multigraf (funkcia z ba-graph).
      Následne treba vybra poèet vrcholov. Graf by mal by kubickı, èie kadı
      sused bude ma 3 susedov. Tım pádom, poèet vrcholov musí by párny.                    
        1.1. po vygenerovaní máte monos zisti èo za graf ste vygenerovali "y"
        1.2. pokia¾ o to nemáte záujem, napíšte "n" (alebo hocièo iné)
    2. "f" = vloi graf z .txt súboru. Súbor by mal by v prieèinku 
      graph_examples. Formát súboru musí by v tvare: prvı riadok obsahuje
      poèet vrcholov, kadı ïalší riadok obsahuje práve jednu hranu (dvojicu 
      vrcholov) - ujistite sa e graf je kubickı, inak program môe zlyha.
        PRIPOMIENKA: nezabudnite zmeni absolútnu cestu
    3. "c" = graf z príkazového riadka. Formát a poiadavky sú rovnaké ako v 
    predchádzajúcom prípade.
    
  Po nastavení vstupného grafu máte monos vybra èi sa vykoná rez grafu ("c"),
    cestová dekompozícia ("d"),  nájdenie 1-pomocnej mnoiny ("o") alebo mnoinu
    vrcholov v èerveno-èiernom grafe("r"). Pokia¾ stlaèíte hocièo iné, 
    automaticky sa vykoná dekompozícia.
  
  V prípade stlaèenia "o" treba navyše nastavi jednu z mnoín rezu.
  V prípade stlaèenia "r" treba navyše urèi ktoré hrany sú èierne a ktoré
    èervené. V prvom riadku treba napísa poèet èiernych hrán, následne v kadom
    ïa¾šom riadku vypísa jednu hranu - dvojicu vrcholov. 
    UPOZORNENIE: Vrcholy musia by v poradí v akom boli definované.
    
  Vo všetkıch algoritmoch je moné nastavi håbku(precíznos). 
    V prvıch dvoch prípadoch (d/c) na stupnici od 1 (povrchové, rıchle) po 15 
    (håbkové, pomalé). V druhıch dvoch prípadoch (o/r) treba nastavi kladnú
    hodnotu reálnemu èíslu epsilon tak, aby následná nerovnica platila.
      - "o": (1/3 + 2*epsilon)*|V0| < ve¾kosRezu
      - "r": |R| > (1/2 + epsilon)*|V|
    
    A. V prípade dekompozície je ako vıstup na prvom riadku vypísaná cestová
      šírka a následne zoznam vriec, t.j. samotná cestová dekompozícia.
      Po jej vypísaní má uívate¾ monos správnos dekompozície overi.
      
    B. V prípade rezu grafu je ako vıstup vypísaná šírka rezu, rezové vrcholy
      (neskôr sú pouité ako parameter pre cestovú dekompozíciu) a samotné
      mnoiny A a B s vrcholmi patriacimi do nich.
      
    C. V prípade 1-pomocnej mnoiny sa na vıstupe vypíše mnoina vrcholov.
      (teoreticky aj prázdna mnoina, pokia¾ neprázdna neexistuje)
    
    D. V prípade èerveno-èiernych hrán sa na vıstupe vypíše mnoina vrcholov. 
      (teoreticky aj prázdna mnoina, pokia¾ neprázdna neexistuje)
      
  Po skonèení dekompozície, rezu grafu, 1-pomocnej mnoiny èi èerveno-èiernych
    hrán má uívate¾ monos skonèi ("quit") alebo pokraèova (hocièo iné, 
    napríklad "nie"). Ak zvolí monos pokraèova, má monos pracova s grafom 
    ktorı bol pouitı naposledy alebo vstupnı graf zmeni (y/n)
    
PRAJEM PRÍJEMNE STRÁVENİ ÈAS A DOBRÉ VİSLEDKY S TİMTO PROJEKTOM :)