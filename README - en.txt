###INSTRUCTIONS FOR USER

This program contains four .hpp (pathDecomposition, cutSizeBisection, 
oneHelpfulSet and redBlackEdges) files and two .cpp files - main.cpp which
makes path decomposition, and test.cpp which tests .hpp files.

###AT FIRST, MAKE SURE YOU HAVE ACCOMPLISHED THESE 3 PREREQUISITIES:

  1. download ba-graph library. You can find it on 
    https://bitbucket.org/relatko/ba-graph/commits/67a1f4e718bd4aaf5a389aa54097db227800f27f.
    If you have troubles with current version, try download version from
    05/07/2019.
      - in Makefile set the path to a library
          - CFLAGS - usually it is ../ba-graph/in-clude/

  2. your compiler should be at least gcc-6 or higher

  3. in case of testing set the absolute path to folder graph_example
        - row 13 in in test.cpp

###INTERFACES

Project provides interfaces for path decomposition, cut of the
  graph, find at least 1-helpful set or find set in red-black graph.
  
- vector<set<Number>> pathDecomposition(Graph &graph);
        - returns sequence of vertices sets belonging to graph
          while it fulfils path decompositions' conditions
    - vector<set<Number>> pathDecomposition(Graph &graph, int epsilonExp)
        - higher epsilonExp means higher precision, lower means lower precision
- array<set<Number>, 4> makeBisection(Graph &graph);
        - returns 4 sets of vertices belonging to graph:
          - makeBisection[0] contains cut-vertices of set V0
          - makeBisection[1] contains vertices of set V0
          - makeBisection[2] contains cut-vertices of set V1
          - makeBisection[3] contains vertices of set V1
    - array<set<Number>, 4> makeBisection(Graph &graph, float epsilonExp)
        - epsilonExp same as in pathDecomposition
- set<Number> getHelpfulSet(Graph &graph, set<Number> v0);
        - returns subset of set of vertices V0 (or V1) which decreases cutsize
          after moving to V1 (V0)
        - in graph we store edges - which are important in this function
    - set<Number> getHelpfulSet(Graph &graph, set<Number> v0, float epsilon)
        - larger epsilon means smaller helpful set and vice versa - for more
          details see taxt of bachelor work
- set<Number> redBlackEdges(Graph &graph, multiset<pair<Number, Number>> &blackEdges);
        - returns set of vertices which fulfills the lemma in respect to a graph
          (far details check a text in bachelor thesis)
        - in multiset blackEdges we store list of black edges
            - WARNING: order of vertices matters!
    - set<Number> redBlackEdges(Graph &graph, multiset<pair<Number, Number>> &blackEdges, float epsilon)
        - similarly as in getHelpfulSet, value of epsilon affects size of set
            we have to find (we skip further details)

###FORMAT OF INPUT IN main.cpp

  In first row of command line is one integer - number of vertices. It should be even because graph is cubic.
  In every next row there is pair of numbers - "u v" means that there is edge between u and v.
    
  After path decomposition, you can choose if you want to quit or continue.
  (enter "quit" or anything else, for example "no")
    
WISH YOU HAVE A GOOD TIME WITH THIS PROJECT :)

     
       
    
  