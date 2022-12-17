/**
 * Kostra programu pro 2. projekt IZP 2022/23
 *
 * Jednoducha shlukova analyza: 2D nejblizsi soused.
 * Single linkage
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <limits.h>

/*****************************************************************
 * Deklarace potrebnych datovych typu:
 *
 *   struct obj_t - struktura objektu: identifikator a souradnice
 *   struct cluster_t - shluk objektu:
 *      pocet objektu ve shluku,
 *      kapacita shluku (pocet objektu, pro ktere je rezervovano
 *      misto v poli),
 *      ukazatel na pole shluku.
 */

struct obj_t {
    int id;
    float x;
    float y;
};

struct cluster_t {
    int size;
    int capacity;
    struct obj_t *obj;
};

/*****************************************************************
 * Deklarace potrebnych funkci.
*****************************************************************/

/// Inicializace shluku 'c'. Alokuje pamet pro cap objektu (kapacitu).
/// Ukazatel NULL u pole objektu znamena kapacitu 0.

void init_cluster(struct cluster_t *c, int cap)
{
    assert(c != NULL);
    assert(cap >= 0);

    // Nastavi hodnoty pro cap = 0
    c->size = 0;
    c->capacity = 0;
    c->obj = NULL;

	if (cap > 0) {
        // Pokud se alokace nezdari
		if((c->obj = malloc(cap * sizeof(struct obj_t))) == NULL){
            fprintf(stderr, "Alokace se nezdarila\n");
            c->obj = NULL;
        }
        
	}
	
}

//// Odstraneni vsech objektu shluku a inicializace na prazdny shluk.

void clear_cluster(struct cluster_t *c)
{
    // Uvolni naalokovanou pamet a inicializuje prazdny cluster
    free(c->obj);
	init_cluster(c, 0);
}

/// Chunk of cluster objects. Value recommended for reallocation.

const int CLUSTER_CHUNK = 10;


/// Zmena kapacity shluku 'c' na kapacitu 'new_cap'.

struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap)
{
    assert(c);
    assert(c->capacity >= 0);
    assert(new_cap >= 0);

    if (c->capacity >= new_cap)
        return c;

    size_t size = sizeof(struct obj_t) * new_cap;

    void *arr = realloc(c->obj, size);
    if (arr == NULL)
        return NULL;

    c->obj = (struct obj_t*)arr;
    c->capacity = new_cap;
    return c;
}


/// Prida objekt 'obj' na konec shluku 'c'.
/// Rozsiri shluk, pokud se do nej objekt nevejde.

void append_cluster(struct cluster_t *c, struct obj_t obj)
{
    if (c->capacity <= c->size) {
		// Rozsireni kapacity shluku
		resize_cluster(c, c->capacity + CLUSTER_CHUNK);
	}

	// Pridani objektu na volnou pozici a zvyseni poctu prvku ve shluku
	c->obj[c->size++] = obj;
}


/// Seradi objekty ve shluku 'c' vzestupne podle jejich identifikacniho cisla.

void sort_cluster(struct cluster_t *c);


/// Do shluku 'c1' prida objekty 'c2'. Shluk 'c1' bude v pripade nutnosti rozsiren.
/// Objekty ve shluku 'c1' budou serazeny vzestupne podle identifikacniho cisla.
/// Shluk 'c2' bude nezmenen.

void merge_clusters(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c2 != NULL);
    
	// Pridani objektu shluku c2 do shluku c1
	for (int i = 0; i < c2->size; i++) {
		append_cluster(c1, c2->obj[i]);
	}

	sort_cluster(c1);
}

/**********************************************************************/
/* Prace s polem shluku */


/// Odstrani shluk z pole shluku 'carr'. Pole shluku obsahuje 'narr' polozek (shluku).
/// Shluk pro odstraneni se nachazi na indexu 'idx'.
/// Funkce vraci novy pocet shluku v poli.

int remove_cluster(struct cluster_t *carr, int narr, int idx)
{
    assert(idx < narr);
    assert(narr > 0);

    // Vymaze shluk na poslednim indexu a nasledne posune vsechny shluky doleva o jednu pozici
    clear_cluster(&carr[idx]);
    for(int i = idx; i < narr - 1; i++){
        carr[i] = carr[i+1];
    }
    
    return narr - 1;
}


/// Pocita Euklidovskou vzdalenost mezi dvema objekty.

float obj_distance(struct obj_t *o1, struct obj_t *o2)
{
    assert(o1 != NULL);
    assert(o2 != NULL);

    float distance = sqrtf(((o1->x - o2->x) * (o1->x - o2->x)) + ((o1->y - o2->y) * (o1->y - o2->y)));
    return distance;
}


/// Pocita vzdalenost dvou shluku.

float cluster_distance(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c1->size > 0);
    assert(c2 != NULL);
    assert(c2->size > 0);

    float minDistance = obj_distance(&c1->obj[0], &c2->obj[0]);
    float temp;

    // Projde vsechny kombinace a vrati nejmensi vzdalenost
    for(int i = 0; i < c1->size; i++){
        for(int j = 0; j < c2->size; j++){
            temp = obj_distance(&c1->obj[i], &c2->obj[j]);
            if(temp < minDistance)
                minDistance = temp;
        }
    }

    return minDistance;
}


/// Funkce najde dva nejblizsi shluky. V poli shluku 'carr' o velikosti 'narr'
/// hleda dva nejblizsi shluky.
/// Nalezene shluky identifikuje jejich indexy v poli 'carr'.
/// Funkce nalezene shluky (indexy do pole 'carr') uklada do pameti na
/// adresu 'c1' resp. 'c2'.


void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2)
{
    assert(narr > 0);

    // Nastavi hodnotu na vzdalenost dvou shluku, at ma s cim porovnavat
    float minDistance = cluster_distance(&carr[0], &carr[1]), temp;

    for(int i = 0; i < narr; i++){  // projede kazdy shluk s kazdym
        for(int j = i + 1; j < narr; j++){
            temp = cluster_distance(&carr[i], &carr[j]);

            if(temp < minDistance){
                minDistance = temp;
                *c1 = i;
                *c2 = j;
            }
        }
    }
}


/// Pomocna funkce pro razeni shluku

static int obj_sort_compar(const void *a, const void *b)
{
    const struct obj_t *o1 = (const struct obj_t *)a;
    const struct obj_t *o2 = (const struct obj_t *)b;
    if (o1->id < o2->id) return -1;
    if (o1->id > o2->id) return 1;
    return 0;
}


/// Razeni objektu ve shluku vzestupne podle jejich identifikatoru.

void sort_cluster(struct cluster_t *c)
{
    qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}


/// Tisk shluku 'c' na stdout.

void print_cluster(struct cluster_t *c)
{
    for (int i = 0; i < c->size; i++)
    {
        if (i) putchar(' ');
        printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
    }
    putchar('\n');
}


/// Uvolni vsechnu naalokovanou pamet

void clear_all(struct cluster_t *arr, int clustersToClean)
{
	for (int i = 0; i < clustersToClean; i++) {
		clear_cluster(&arr[i]);
	}
	free(arr);
}


/// Naalokuje pamet pro potrebne mnozstvi clusteru

void init_cluster_array(struct cluster_t **arr, int numberOfClusters)
{
	// Alokace pameti pro pole shluku
	if (!(*arr = malloc(numberOfClusters * sizeof(struct cluster_t)))) {
		return;
	}
	// Inicializace objektu pole
	for (int i = 0; i < numberOfClusters; i++) {
		init_cluster(&(*arr)[i], 0);
	}
}


/// Ze souboru 'filename' nacte objekty.
/// Pro kazdy objekt vytvori shluk a ulozi jej do pole shluku.
/// Alokuje prostor pro pole vsech shluku a ukazatel na prvni
/// polozku pole (ukalazatel na prvni shluk v alokovanem poli)
/// ulozi do pameti, kam se odkazuje parametr 'arr'.
/// Funkce vraci pocet nactenych objektu (shluku).
/// V pripade nejake chyby uklada do pameti kam se odkazuje 'arr' hodnotu NULL.

int load_clusters(char *filename, struct cluster_t **arr)
{
    assert(arr != NULL);
    
    FILE *fptr = fopen(filename, "r");
    if(fptr == NULL){fprintf(stderr, "Nepodarilo se otevrit soubor\n"); return -1;}

    int id, x, y, line = 0, objectsNeeded = 0;
    
    int lineLen = 12;
    char buffer[lineLen], endchar;

    struct obj_t o;
    struct cluster_t *cluster;

    // Cte soubor radek po radku
    while (fgets(buffer, lineLen, fptr) && ++line)
    {
        // Pokud se jedna o prvni radek, zjisti kolik zde ma byt objektu
        if(line == 1){

            // Osetri, ze nacte pouze pocet objektu
            if(sscanf(buffer, "count=%d%[^\n]", &objectsNeeded, &endchar) != 1){
                fprintf(stderr, "Nepovedlo se nacist pocet objektu\n");
                fclose(fptr);
                return -1;
            }

            // Osetri, ze je zde vice nez 0 objektu
            if(objectsNeeded <= 0){
                fprintf(stderr, "Pocet objektu musi byt vetsi nez 0\n");
                fclose(fptr);
                return -1;
            }

            // Inicializovani pole clusteru
            init_cluster_array(arr, objectsNeeded);
            if(! *arr){
                fprintf(stderr, "Nepovedlo se alokovat pamet\n");
                fclose(fptr);
                return -1;
            }
            continue;
        }
        
        // Osetri nepovolene mnozstvi
        if(line - 1 > objectsNeeded){
            fprintf(stderr, "Nepovoleny pocet objektu\n");
            fclose(fptr);
            clear_all(*arr, objectsNeeded);

            *arr = NULL;
            return -1;
        }

        // Prevedeni nacteneho radku do Intovych promennych a zaroven osetrovani vstupnich podminek
        if(sscanf(buffer, "%d %d %d%[^\n]", &id, &x, &y, &endchar) != 3 || x < 0 || x > 1000 || y < 0 || y > 1000){
            fprintf(stderr, "Nepovolena vstupni data\n");
            fclose(fptr);
            clear_all(*arr, objectsNeeded);
            *arr = NULL;
            return -1;
        }

        // Nacitani promennych do objektu pro nasledne ulozeni do shluku
        o.id = id;
        o.x = x;
        o.y = y;

        cluster = &(*arr)[line - 2];
        append_cluster(cluster, o);

        // Po provedeni funkce se vzdy musi zvetsit size na 1, a pokud ne, tak selhala alokace
        if(cluster->size != 1){
            fprintf(stderr, "Alokace pameti se nezdarila\n");
            fclose(fptr);
            clear_all(*arr, objectsNeeded);
            *arr = NULL;
            return -1;
        }
    }
    fclose(fptr);

    // Osetri nepovolene mnozstvi objektu
    if(line - 1 < objectsNeeded){
        fprintf(stderr, "Nepovoleny pocet objektu\n");
        clear_all(*arr, objectsNeeded);
        *arr = NULL;
        return -1;
    }

    return objectsNeeded;
}


/// Tisk pole shluku. Parametr 'carr' je ukazatel na prvni polozku (shluk).
/// Tiskne se prvnich 'narr' shluku.

void print_clusters(struct cluster_t *carr, int narr)
{
    printf("Clusters:\n");
    for (int i = 0; i < narr; i++)
    {
        printf("cluster %d: ", i);
        print_cluster(&carr[i]);
    }
}

int main(int argc, char *argv[])
{
    struct cluster_t *clusters = NULL;
    char *filename;
    int numberOfClustersWanted, amountOfClusters, c1, c2;

    // Osetreni vstupnich hodnot
    if(argc == 2){
        // Pokud neni specifikovano, pocet chtenych shluku je roven 1
        numberOfClustersWanted = 1;
        filename = argv[1];
    }
    else if(argc == 3){
        char *endpointer = NULL;

        filename = argv[1];
        numberOfClustersWanted = strtol(argv[2], &endpointer, 10);

        if(*endpointer){
            fprintf(stderr, "Nespravne zadane argumenty\n");
            return -1;
        }

        if(numberOfClustersWanted <= 0){
            fprintf(stderr, "Hodnota musi byt vetsi nez 0\n");
            return -1;
        }
    }
    else{
        fprintf(stderr, "Nespravne zadane argumenty\n");
        return -1;
    }

    if((amountOfClusters = load_clusters(filename, &clusters)) == -1){
        return -1;
    }

    if(numberOfClustersWanted > amountOfClusters){
        fprintf(stderr, "Pozadovany pocet je vetsi nez pocet dostupnych shluku\n");
        return -1;
    }

    // Pobezi, dokud nedostaneme pozadovany pocet shluku
    while(amountOfClusters > numberOfClustersWanted){
        find_neighbours(clusters, amountOfClusters, &c1, &c2);
        merge_clusters(&clusters[c1], &clusters[c2]);
        amountOfClusters = remove_cluster(clusters, amountOfClusters, c2);
    }
    
    print_clusters(clusters, amountOfClusters);
    clear_all(clusters, amountOfClusters);

    return 0;
}
