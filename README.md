# Cluster Analysis
Metoda nejbližšího souseda vybírá ke spojení vždy dva shluky, které mají k sobě nejblíže.

Vzdálenost dvou shluků je rovna nejmenší vzdálenosti libovolných dvou objektů z obou shluků.

## Spousteni programu

### Preklad
Program se preklada s temito argumenty:

gcc -std=c99 -Wall -Wextra -Werror -DNDEBUG cluster.c -o cluster -lm

### Spousteni
Program se spousti nasledovne:

./cluster SOUBOR [N]

- SOUBOR je jméno souboru se vstupními daty.
- N je volitelný argument definující cílový počet shluků. N > 0. Výchozí hodnota (při absenci argumentu) je 1.

### Priklad spousteni a vystupu

$ cat objekty  
count=20  
40 86 663  
43 747 938  
47 285 973  
49 548 422  
52 741 541  
56 44 854  
57 795 59  
61 267 375  
62 85 874  
66 125 211  
68 80 770  
72 277 272  
74 222 444  
75 28 603  
79 926 463  
83 603 68  
86 238 650  
87 149 304  
89 749 190  
93 944 835  

$ ./cluster objekty 8  
Clusters:  
cluster 0: 40[86,663] 56[44,854] 62[85,874] 68[80,770] 75[28,603] 86[238,650]  
cluster 1: 43[747,938]  
cluster 2: 47[285,973]  
cluster 3: 49[548,422]  
cluster 4: 52[741,541] 79[926,463]  
cluster 5: 57[795,59] 83[603,68] 89[749,190]  
cluster 6: 61[267,375] 66[125,211] 72[277,272] 74[222,444] 87[149,304]  
cluster 7: 93[944,835]  

## Fungovani programu

1. Zjisti, kolik je v souboru clusteru
2. Alokuje pro ne potrebou pamet
3. Vytvori cluster pro kazdy objekt a ulozi jej do pole clusteru
4. Bezi tak dlouho, dokud neni dosazen pozadovany pocet shluku:
- Najde nejblizsi sousedy
- Prida shluk 2 do shluku 1
- Odstrani shluk 2
5. Vypise pole slusteru
6. Uvolni veskerou alokovanou pamet
