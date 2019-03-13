 /** @file
 * Implementacja modułu phone_forward.h
 *
 * @author Jan Klinkosz <jk394342@mimuw.edu.pl>
 * @date 12.05.2018
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "phone_forward.h"

///znak zero
#define zero '0'

/**
 * Wartość przedstawiająca liczbę dostępnych cyfr
 */
#define numberOfDigits 12

///////
///////
///////
// Phone_num

/** @brief Kopiuje numer.
 * Tworzy nowy numer, który jest taki sam jak ten wskazany przez @p num.
 *
 * @param[in] num – wskaźnik na poprawny numer.
 * @return Wskaźnik na skopiowany numer.
 */

static char const *copy_number(char const *num)
{
    size_t len = strlen(num);

    char *new_num = malloc(sizeof(char const) * len + 1);

    new_num = strcpy(new_num, num);

    return new_num;
}

/** @brief Łączy dwa numery w jeden ( @p num1 + @p num2).
 * Tworzy nowy numer, który jest konkatenacją numerów
 * wskazanych przez @p num1 i @p num2.
 *
 * @param[in] num1 – wskaźnik na poprawny numer.
 * @param[in] num2 – wskaźnik na poprawny numer.
 * @return Wskaźnik na stworzony numer.
 */

static char const *merge_numbers(char const *num1, char const *num2)
{
    size_t len1 = strlen(num1), len2 = strlen(num2);

    char *new_num = malloc(sizeof(char const) * (len1 + len2 + 1));

    for(unsigned int i = 0; i < len1; i++)
    {
        new_num[i] = num1[i];
    }
    for(unsigned int i = len1; i < len1 + len2; i++)
    {
        new_num[i] = num2[i - len1];
    }

    new_num[len1 + len2] = '\0';

    return new_num;
}


/** @brief Tworzy nową strukturę PhoneNumbers o rozmiarze @p size.
 *
 * @param[in] size – początkowy rozmiar struktury większy niż 0.
 * @return Wskaźnik na stworzoną strukturę.
 */

static struct PhoneNumbers *phnumNew(int size)
{
    struct PhoneNumbers *numbers = malloc(sizeof(struct PhoneNumbers));

    if (numbers == NULL)
        return NULL;

    numbers->struct_size = size;
    numbers->size = 0;
    numbers->tab = malloc(sizeof(char* const) * size);

    return numbers;
}


/** @brief Dodaje numer do struktury.
 *  Tworzy nowy numer, taki sam jak ten wskazywany przez @p num
 *  i dodaje go (jego wskaźnik) do struktury @p pnum.
 *
 * @param[in,out] pnum – wskaźnik na strukture przechowującą numery.
 * @param[in] num – wskaźnik na numer
 * @return Wskaźnik na stworzoną strukturę z dodanym numerem, jeśli dodano numer.
 *         Wskaźnik na strukturę bez dodanego numeru, jeżel nie udało się zaalokować pamięci.
 */
static struct PhoneNumbers *phnumAdd(struct PhoneNumbers *pnum, char const *num)
{

    if (pnum->size == pnum->struct_size)
    {
        int new_size = pnum->struct_size * 2;

        struct PhoneNumbers *pnum2 = pnum;
        pnum = phnumNew(new_size);

        if (pnum == NULL)
            return pnum2;

        pnum->size = pnum2->size;

        for(int i = 0; i < pnum2->size; i++)
        {
            pnum->tab[i] = pnum2->tab[i];
        }

        free((char **)pnum2->tab);
        free((void *)pnum2);
    }

    char const *new_num = copy_number(num);
    pnum->tab[pnum->size] = new_num;
    pnum->size++;

    return pnum;
}

/** @brief Usuwa numer o zadanym indeksie ze struktury.
 *  Usuwa numer o indeksie @p idx ze struktury @p pnum.
 *
 * @param[in,out] pnum – wskaźnik na strukture przechowującą numery.
 * @param[in] idx – indeks numeru w strukturze (idx >= 0 && idx < liczba numerów w strukturze) .
 * @return Wskaźnik na strukturę z usuniętym numerem, jeśli usunięto numer.
 *         Wskaźnik na strukturę bez usuniętego numeru, jeżel nie udało się zaalokować pamięci.
 */

struct PhoneNumbers *phnumRemove(struct PhoneNumbers *pnum, int idx)
{
    free((char *)pnum->tab[idx]);
    pnum->tab[idx] = pnum->tab[pnum->size - 1];
    pnum->size--;

    if (pnum->size == pnum->struct_size/2 && pnum->size != 0)
    {
        int new_size = pnum->size;

        struct PhoneNumbers *pnum2 = pnum;
        pnum = phnumNew(new_size);

        if (pnum == NULL)
            return pnum2;

        pnum->size = pnum2->size;

        for(int i = 0; i < pnum2->size; i++)
        {
            pnum->tab[i] = pnum2->tab[i];
        }

        free((char **)pnum2->tab);
        free((void *)pnum2);
    }

    return pnum;
}


// Funkcja z phone_forward.h
char const *phnumGet(struct PhoneNumbers const *pnum, size_t idx)
{
    if ((signed int)pnum->size > (signed int)idx)
        return pnum->tab[idx];
    else
        return NULL;
}

/** @brief Komparator numerów.
 *
 * @param[in] a – wskaźnik na numer.
 * @param[in] b – wskaźnik na numer.
 * @return Wartość mniejszą od 0, jeżeli a < b (w leksykograficznym porządku numerów pod tymi wskaźnikami).
 *         Wartość 0, jeżeli a = b.
 *         Wartość większą od 0, jeżeli a > b.
 */

static int compare(const void *a, const void *b)
{
    return strcmp(*(const char **) a, *(const char **) b);
}

/** @brief Sortuje numery w strukturze @p pnum.
 *
 *  Sortuje n pierwszych numerów w strukturze @p pnum
 *
 * @param[in] pnum – wskaźnik na strukture przechowującą numery.
 * @param[in] n – liczba elementów do posortowania.
 */

static void phnumSort(struct PhoneNumbers *pnum, int n)
{
    qsort(pnum->tab, n, sizeof(const char*), compare);
}


/** @brief Usuwa powtarzające się numery w strukturze @p pnum.
 *
 * @param[in,out] pnum – wskaźnik na strukture przechowującą numery.
 * @return Wskaźnik, która zawiera wszystkie numery ze struktury
 *         @p pnum bez powtórzeń.
 */

static struct PhoneNumbers *phnumUnique(struct PhoneNumbers *pnum)
{
    phnumSort(pnum, pnum->size);

    for(int i = pnum->size - 1; i > 0; i--)
    {
        if (strcmp(pnum->tab[i], pnum->tab[i-1]) == 0)
            pnum = phnumRemove(pnum, i);
    }

    phnumSort(pnum, pnum->size);

    return pnum;

}

///////
///////
///////
// Trie_forward

/** @brief Tworzy pusty obiekt typu Trie_forward
 *
 * Tworzy pusty obiekt typu Trie_forward, domyślnie ustawia
 * wskaźniki na synów i forwarding jako NULL.
 *
 * @return Pusty obiekt typu Trie_forward.
 */

static Trie_forward trieforNew()
{
    Trie_forward t = (Trie_forward)malloc(sizeof(struct Node_forward));

    t->forwarding = NULL;
    // Jest numberOfDigits cyfr
    t->sons = malloc(sizeof(Trie_forward) * numberOfDigits);
    t->numberOfSons = 0;

    for(int i = 0; i < numberOfDigits; i++)
    {
        t->sons[i] =  NULL;
    }

    return t;
}

/** @brief Dealokuje pojedynczy wierzchołek typu Trie_forward
 *
 * @param[in] t – obiekt typu Trie_forward.
 */

static void trieforDeleteNode(Trie_forward t)
{
    free(t->sons);
    free(t->forwarding);
    free(t);
}

/** @brief Dealokuje pojedynczy wierzchołek i całe poddrzewo typu Trie_forward
 *
 * @param[in] t – obiekt typu Trie_forward.
 */

static void trieforDelete(Trie_forward t)
{
    for(int i = 0; i < numberOfDigits; i++)
    {
        if (t->sons[i] != NULL)
            trieforDelete(t->sons[i]);
    }

    trieforDeleteNode(t);
}


/** @brief Dodaje przekierowanie.
 *
 * Dodaje przekierowanie z numeru wskazywanego przez @p num1
 * na numer wskazywany przez @p num2 do @p t.
 *
 * @param[in] t – obiekt typu Trie_forward.
 * @param[in] num1 – wskaźnik na numer przekierowywany.
 * @param[in] num2 – wskaźnik na numer, który jest przekierowniem z @p num1
 * @return Wartość @p true, jeśli przekierowanie zostało dodane
 *         Wartość @p false, jeśli przekierowanie nie zostało dodane
 */

static bool trieforAdd(Trie_forward t, char *num1, char *num2)
{
    bool p;

    if (num1[0] == '\0')
    {
        if (t->forwarding != NULL)
            free(t->forwarding);

        t->forwarding = (char  *)copy_number(num2);
        p = true;
    }
    else
    {
        if (t->sons[num1[0] - zero] == NULL)
        {
            t->sons[num1[0] - zero] = trieforNew();
            t->numberOfSons++;
        }

        if (t->sons[num1[0] - zero] == NULL)
            p = false;
        else
            p = trieforAdd(t->sons[num1[0] - zero], num1 + 1, num2);
    }

    return p;
}

/** @brief Usuwa wierzchołek @p t i jego poddrzewo.
 *
 * Usuwa wierzchołek @p t i jego poddrzewo, a przekierowanie, które
 * znajdowały się w usuwanych wierzchołkach dodaje do struktury
 * skazywanej przez @p numbers.
 *
 * @param[in] t – obiekt typu Trie_forward.
 * @param[in] numbers – wskaźnik na strukturę przechowującą numery.
 * @return Wskaźnik na strukturę @p numbers z dodanymi numerami.
 */

static struct PhoneNumbers *removeSubtree(Trie_forward t, struct PhoneNumbers *numbers)
{
    for(int i = 0; i < numberOfDigits; i++)
    {
        if (t->sons[i] != NULL)
            numbers = removeSubtree(t->sons[i], numbers);
    }

    if (t->forwarding != NULL)
        numbers = phnumAdd(numbers, t->forwarding);

    trieforDeleteNode(t);

    return numbers;
}

/** @brief Usuwa przekierowania z obiektu typu Trie_forward.
 *
 * Usuwa wszystkie przekierowania, w których parametr @p num jest prefiksem
 * parametru @p num1 użytego przy dodawaniu. Jeśli nie ma takich przekierowań
 * lub napis nie reprezentuje numeru, nic nie robi.  Alokuje strukturę
 * @p PhoneNumbers,która musi być zwolniona za pomocą funkcji @ref phnumDelete.
 *
 * @param[in] t – obiekt typu Trie_forward.
 * @param[in] num – wskaźnik na numer.
 * @return Wskaźnik na strukturę przechowującą numery,
 *         które były przekierowaniami w usuwanych wierzchołkach.
 */

static struct PhoneNumbers *trieforRemove(Trie_forward t, char *num)
{

    bool removeSub = true;

    struct PhoneNumbers *numbers = phnumNew(4);

    char *numAux = num;
    Trie_forward tAux = t;
    unsigned int depth = 0, indeks = 0;
    size_t len = strlen(num);

    // Schodzenie po drzewie i spamiętywanie co można usunąć

    while(numAux[0] != '\0')
    {

        if (t->sons[numAux[0] - zero] != NULL)
        {
            if (len-1 == indeks)
            {
                if (tAux == NULL)
                {
                    tAux = t;
                    depth = indeks;
                }

            }
            else
            {

                if (t->sons[numAux[0] - zero]->forwarding != NULL)
                    tAux = NULL;
                else
                {
                    if (t->sons[numAux[0] - zero]->numberOfSons == 1)
                    {
                        if (tAux == NULL)
                        {
                            tAux = t;
                            depth = indeks;
                        }
                    }
                    else
                        tAux = NULL;
                }

            }

            t = t->sons[numAux[0] - zero];
        }
        else
        {
            removeSub = false;
            break;
        }

        indeks++;
        numAux++;
    }

    //Usuwanie poddrzewa, które można usunąć

    if (removeSub)
    {
        numbers = removeSubtree(tAux->sons[num[depth] - zero], numbers);
        tAux->sons[num[depth] - zero] = NULL;
        tAux->numberOfSons--;
    }

    return numbers;
}


// Dostaje już poprawny numer

/** @brief Wyznacza przekierowanie numeru.
 * Wyznacza przekierowanie podanego numeru. Szuka najdłuższego pasującego
 * prefiksu. Wynikiem jest co najwyżej jeden numer. Jeśli dany numer nie został
 * przekierowany, to wynikiem jest ten numer. Jeśli podany napis nie
 * reprezentuje numeru, wynikiem jest pusty ciąg. Alokuje strukturę
 * @p PhoneNumbers,która musi być zwolniona za pomocą funkcji @ref phnumDelete.
 * @param[in] t  – obiekt typu Trie_forward;
 * @param[in] num – wskaźnik na napis reprezentujący numer.
 * @return Wskaźnik na strukturę przechowującą ciąg numerów lub NULL, gdy nie
 *         udało się zaalokować pamięci.
 */

static struct PhoneNumbers *trieforGet(Trie_forward t, char *num)
{
    struct PhoneNumbers *number = phnumNew(1);

    char *s = NULL;
    char *numAux = num;
    char *numAux2;

    while(numAux[0] != '\0')
    {
        if (t->sons[numAux[0] - zero] != NULL)
        {

            if (t->sons[numAux[0] - zero]->forwarding != NULL)
            {
                s = t->sons[numAux[0] - zero]->forwarding;
                numAux2 = numAux + 1;
            }

            t = t->sons[numAux[0] - zero];
        }
        else
            break;

        numAux++;
    }


    if (s == NULL)
        s = (char *)copy_number(num);
    else
        s = (char *)merge_numbers(s, numAux2);

    number = phnumAdd(number, s);
    free(s);

    return number;
}

/** @brief Zwraca przekierowanie numeru wskazanego przez @p num.
 *
 * Tworzy nowy numer i wskaźnik na niego jako przekierowanie @p num.
 *
 * @param[in] t – obiekt typu Trie_forward.
 * @param[in] num – wskaźnik na numer.
 * @return Wskaźnik na numer, jeżeli przekierowanie istnieje.
 *         NULL w przeciwnym wypadku.
 */

static const char * trieforGetForward(Trie_forward t, char *num)
{
    char *numAux = num;

    while(numAux[0] != '\0')
    {
        if (t->sons[numAux[0] - zero] == NULL)
            return NULL;
        else
        {
            t = t->sons[numAux[0] - zero];
            numAux++;
        }
    }

    if (t->forwarding == NULL)
        return NULL;
    else
        return copy_number(t->forwarding);
}

////
////
////
//Trie_reverse

/** @brief Tworzy pusty obiekt typu Trie_reverse
 *
 * Tworzy pusty obiekt typu Trie_reverse, domyślnie ustawia
 * wskaźniki na synów i forwarding jako NULL.
 *
 * @return Pusty obiekt typu Trie_forward.
 */

static Trie_reverse trierevNew()
{
    Trie_reverse t = (Trie_reverse)malloc(sizeof(struct Node_reverse));

    t->reverse = phnumNew(1);
    // Jest numberOfDigits cyfr
    t->sons = malloc(sizeof(Trie_reverse) * numberOfDigits);
    t->numberOfSons = 0;

    for(int i = 0; i < numberOfDigits ; i++)
    {
        t->sons[i] =  NULL;
    }

    return t;
}

/** @brief Dealokuje pojedynczy wierzchołek typu Trie_reverse
 *
 * @param[in] t – obiekt typu Trie_reverse.
 */

static void trierevDeleteNode(Trie_reverse t)
{
    phnumDelete(t->reverse);
    free(t->sons);
    free(t);
}

/** @brief Dealokuje pojedynczy wierzchołek i całe poddrzewo typu Trie_reverse
 *
 * @param[in] t – obiekt typu Trie_reverse.
 */

static void trierevDelete(Trie_reverse t)
{
    for(int i = 0; i < numberOfDigits; i++)
    {
        if (t->sons[i] != NULL)
            trierevDelete(t->sons[i]);
    }
    trierevDeleteNode(t);
}


/** @brief Dodaje przekierowanie.
 *
 * Dodaje przekierowanie z numeru wskazywanego przez @p num1
 * na numer wskazywany przez @p num2 do @p t.
 *
 * @param[in] t – obiekt typu Trie_reverse.
 * @param[in] num1 – wskaźnik na numer przekierowywany.
 * @param[in] num2 – wskaźnik na numer, który jest przekierowniem z @p num1
 * @return Wartość @p true, jeśli przekierowanie zostało dodane
 *         Wartość @p false, jeśli przekierowanie nie zostało dodane
 */

static bool trierevAdd(Trie_reverse t, char *num1, char *num2)
{
    bool p;

    if (num1[0] == '\0')
    {
        t->reverse = phnumAdd(t->reverse, num2);
        p = true;
    }
    else
    {
        if (t->sons[num1[0] - zero] == NULL)
        {
            t->sons[num1[0] - zero] = trierevNew();
            t->numberOfSons++;
        }

        if (t->sons[num1[0] - zero] == NULL)
            p = false;
        else
            p = trierevAdd(t->sons[num1[0] - zero], num1 + 1, num2);
    }

    return p;
}


/** @brief Usuwa przekierowania z obiektu typu Trie_reverse.
 *
 * Usuwa wszystkie przekierowania numerów ze struktury @p pnum,
 * w których parametr @p num jest prefiksem
 * parametru @p num1 użytego przy dodawaniu. Jeśli nie ma takich przekierowań
 * lub napis nie reprezentuje numeru, nic nie robi.
 *
 * @param[in] trev – obiekt typu Trie_reverse.
 * @param[in] num – wskaźnik na numer.
 * @param[in] pnum – wskaźnik na strukturę przechowującą numery.
 * @return Wskaźnik na strukturę przechowującą numery,
 *         które były przekierowaniami w usuwanych wierzchołkach.
 */

static void trierevRemove(Trie_reverse trev, struct PhoneNumbers *pnum, const char *num)
{
    bool removeSub;
    char *numAux;
    Trie_reverse tAux = NULL, t = trev;
    unsigned int depth, indeks = 0;
    size_t len = strlen(num);

    for(int k = 0; k < pnum->size; k++)
    {
        numAux = (char *)pnum->tab[k];
        tAux = trev;
        t = trev;
        depth = 0;
        indeks = 0;
        len = strlen(pnum->tab[k]);
        removeSub = true;

        // Schodzenie w drzewie i spamiętywanie co można usunąć

        while(numAux[0] != '\0')
        {
            // Sprawdzam, czy już usunąłem to
            if (t->sons[numAux[0] - zero] != NULL)
            {
                if (len-1 == indeks)
                {
                    if (tAux == NULL)
                    {
                        tAux = t;
                        depth = indeks;
                    }
                }
                else
                {
                    if (t->sons[numAux[0] - zero]->reverse->size > 0)
                        tAux = NULL;
                    else
                    {
                        if (t->reverse->size == 0 && (t->sons[numAux[0] - zero]->numberOfSons == 1 || t->sons[numAux[0] - zero]->numberOfSons == 0) )
                        {
                            if (tAux == NULL)
                            {
                                tAux = t;
                                depth = indeks;
                            }
                        }
                        else
                            tAux = NULL;
                    }
                }

                t = t->sons[numAux[0] - zero];

                indeks++;
                numAux++;
            }
            else
            {
                removeSub = false;
                break;
            }
        }

        if (removeSub)
        {

            // chcemy usunąć z tablicy reverse rzeczy, które musimy

            int idx = 0, iter;
            int *tab = malloc(sizeof(int) * t->reverse->size);

            for(int i = 0; i < t->reverse->size; i++)
            {
                iter = 0;

                while(num[iter] != '\0' && t->reverse->tab[i][iter] != '\0')
                {
                    if (num[iter] != t->reverse->tab[i][iter])
                    {
                        iter = 0;
                        break;
                    }
                    iter++;
                }

                if (num[iter] == '\0')
                {
                    tab[idx] = i;
                    idx++;
                }
            }

            for(int i = idx - 1; i >= 0; i--)
            {
                t->reverse = phnumRemove(t->reverse, tab[i]);
            }

            free(tab);

            //Usuwanie poddrzewa

            if (t->reverse->size == 0 && t->numberOfSons == 0)
            {
                trierevDelete(tAux->sons[pnum->tab[k][depth] - zero]);
                tAux->sons[pnum->tab[k][depth] - zero] = NULL;
                tAux->numberOfSons--;
            }

        }

    }
}


/** @brief Usuwa jendo przekierowanie z obiektu typu Trie_reverse.
 *
 * Usuwa przekierowanie z numeru wskazywanego przez @p num
 * na numer wskazywany przez @p num2
 *
 * @param[in] trev – obiekt typu Trie_reverse.
 * @param[in] num – wskaźnik na numer, z którego przekierowujemy.
 * @param[in] num2 – wskaźnik na numer, który jest przekirowanie.
 * @return Wskaźnik na strukturę przechowującą numery,
 *         które były przekierowaniami w usuwanych wierzchołkach.
 */

static void trierevRemoveOne(Trie_reverse trev, const char *num, const char *num2)
{
    bool removeSub;
    char *numAux;
    Trie_reverse tAux = NULL, t = trev;
    unsigned int depth, indeks = 0;
    size_t len;

    numAux = (char *)num;
    tAux = trev;
    t = trev;
    depth = 0;
    indeks = 0;
    len = strlen(num);
    removeSub = true;

    while(numAux[0] != '\0')
    {
        // Sprawdzam, czy już usunąłem to
        if (t->sons[numAux[0] - zero] != NULL)
        {
            if (len-1 == indeks)
            {
                if (tAux == NULL)
                {
                    tAux = t;
                    depth = indeks;
                }
            }
            else
            {
                if (t->sons[numAux[0] - zero]->reverse->size > 0)
                    tAux = NULL;
                else
                {
                    if (t->reverse->size == 0 && (t->sons[numAux[0] - zero]->numberOfSons == 1 || t->sons[numAux[0] - zero]->numberOfSons == 0) )
                    {
                        if (tAux == NULL)
                        {
                            tAux = t;
                            depth = indeks;
                        }
                    }
                    else
                        tAux = NULL;
                }
            }

            t = t->sons[numAux[0] - zero];

            indeks++;
            numAux++;
        }
        else
        {
            removeSub = false;
            break;
        }
    }

    if (removeSub)
    {
        // chcemy usunąć z tablicy reverse rzeczy, które musimy

        int idx = 0;

        for(int i = 0; i < t->reverse->size; i++)
        {
            if (strcmp(num2, t->reverse->tab[i]) == 0)
            {
                idx = i;
                break;
            }
        }

        t->reverse = phnumRemove(t->reverse, idx);

        //Usuwanie poddrzewa

        if (t->reverse->size == 0 && t->numberOfSons == 0)
        {
            trierevDelete(tAux->sons[num[depth] - zero]);
            tAux->sons[num[depth] - zero] = NULL;
            tAux->numberOfSons--;
        }

    }
}

/** @brief Wyznacza przekierowania na prefiksy danego numeru w drzewie @p t.
 * Wyznacza wszystkie przekierowania na pefiksy danego numeru @p num. Wynikowy ciąg zawiera też
 * dany numer. Wynikowe numery są posortowane leksykograficznie i nie mogą się
 * powtarzać. Jeśli podany napis nie reprezentuje numeru, wynikiem jest pusty
 * ciąg. Alokuje strukturę @p PhoneNumbers, która musi być zwolniona za pomocą
 * funkcji @ref phnumDelete.
 * @param[in] t  – obiket typu Trie_reverse.
 * @param[in] num – wskaźnik na napis reprezentujący numer.
 * @return Wskaźnik na strukturę przechowującą ciąg numerów lub NULL, gdy nie
 *         udało się zaalokować pamięci.
 */

static struct PhoneNumbers *trierevReverse(Trie_reverse t, char *num)
{
    struct PhoneNumbers *numbers = phnumNew(2);
    numbers = phnumAdd(numbers, num);
    char *numAux = num;

    while(numAux[0] != '\0')
    {

        if (t->sons[numAux[0] - zero] != NULL)
        {
            t = t->sons[numAux[0] - zero];

            char *numAux2;

            for(int i = 0; i < t->reverse->size; i++)
            {
                numAux2 = (char *)merge_numbers(t->reverse->tab[i], numAux + 1);
                numbers = phnumAdd(numbers, numAux2);
                free(numAux2);
            }
        }
        else
            break;

        numAux++;
    }

    numbers = phnumUnique(numbers);

    return numbers;
}


// PhoneForward

/** @brief Sprawdza, czy @p s wskazuje na poprawny numer.
 *
 * @param[in] s  – wskaźnik na ciąg znaków.
 * @return Wartość @p true, jeżeli @p s wskazuje na poprawny numer.
 *         Wartośc @p false w przeciwnym wypadku.
 */

static bool is_number(char const *s)
{
    if (s == NULL)
        return false;

    size_t len = strlen(s);

    for(unsigned int i = 0; i < len; i++)
    {
        if (s[i] < zero || s[i] > zero + numberOfDigits - 1)
            return false;
    }

    return (len > 0);
}



// Funkcje z phone_forward.h

struct PhoneForward *phfwdNew()
{
    struct PhoneForward *t = malloc(sizeof(struct PhoneForward));
    t->tfor = trieforNew();
    t->trev = trierevNew();

    return t;
}

void phfwdDelete(struct PhoneForward *pf)
{
    if (pf != NULL)
    {
        trieforDelete(pf->tfor);
        trierevDelete(pf->trev);

        free(pf);
    }
}

bool phfwdAdd(struct PhoneForward *pf, char const *num1, char const *num2)
{
    if (pf == NULL)
        return false;

    if (!is_number(num1))
        return false;

    if (!is_number(num2))
        return false;

    if (strcmp(num1, num2) == 0)
        return false;

    const char *num = trieforGetForward(pf->tfor, (char *)num1);

    if (num != NULL)
    {
        trierevRemoveOne(pf->trev, (char *)num, (char *)num1);
        free((char *)num);
    }

    if (!trieforAdd(pf->tfor, (char *)num1, (char *)num2))
        return false;

    return trierevAdd(pf->trev, (char *)num2, (char *)num1);
}

void phfwdRemove(struct PhoneForward *pf, char const *num)
{
    if (is_number(num) && pf != NULL)
    {
        struct PhoneNumbers *pnum;
        pnum = trieforRemove(pf->tfor, (char *)num);
        trierevRemove(pf->trev, pnum, (char *)num);
        phnumDelete(pnum);
    }
}

struct PhoneNumbers const * phfwdGet(struct PhoneForward *pf, char const *num)
{
    if (is_number(num) && pf != NULL)
        return trieforGet(pf->tfor, (char *)num);

    return phnumNew(1);

}

struct PhoneNumbers const * phfwdReverse(struct PhoneForward *pf, char const *num)
{
    if (is_number(num) && pf != NULL)
        return trierevReverse(pf->trev, (char *)num);

    return phnumNew(1);
}

// NonTrivialCount

/** @brief Podnosi @p basis do potęgi @p idx.
 *
 * @param[in] basis  – podstawa.
 * @param[in] idx  – wykładnik.
 * @return Zwraca wartość modulo rozmiar zakres size_t.
 */

size_t power(size_t basis, size_t idx)
{
    size_t result = 1, aux = basis;

    while(idx > 0){

        if (idx % 2 == 1)
            result *= aux;

        aux*=aux;
        idx /= 2;
    }

    return result;
}

/** @brief Wylicza liczbę nietrywialnych numerów, którą wnioskuje z poddrzewa.
 *
 * @param[in] trev  – wskażnik na strukturę Trie_Reverse.
 * @param[in] len  – długość numerów.
 * @param[in] basis  – liczba różnych cyfr, które rozpatrujemy.
 * @param[in] setDigits  – tablica z wartościami, które wskazują, które cyfry są w secie.
 * @param[in] depth  – Aktualne głębokośc dfs - a.
 *
 * @return Wylicza liczbę nietrywialnych numerów, którą wnioskuje z poddrzewa.
 */

size_t dfsNonTrivialCount(Trie_reverse trev, size_t len, size_t basis, bool *setDigits, size_t depth)
{
    size_t res = 0;

    if (len < depth)
        return 0;

    if (trev->reverse->size > 0)
        return power(basis, len - depth);

    for(int i = 0; i < numberOfDigits; i++)
    {
        if (setDigits[i] && trev->sons[i] != NULL)
            res += dfsNonTrivialCount(trev->sons[i], len, basis, setDigits, depth + 1);
    }

    return res;
}

//komentarz w phone_forward.h
size_t phfwdNonTrivialCount(struct PhoneForward *pf, char const *set, size_t len)
{
    // Sprawdza jakie cyfry są w napisie set
    bool setDigits[numberOfDigits];
    char *numAux = (char *)set;
    size_t setNumberOfDigits = 0;

    for(int i = 0; i < numberOfDigits; i++)
        setDigits[i] = false;

    while(numAux[0] != '\0')
    {
        if (!setDigits[numAux[0] - zero] && numAux[0] >= zero && numAux[0] < zero + numberOfDigits)
        {
            setDigits[numAux[0] - zero] = true;
            setNumberOfDigits++;
        }

        numAux++;
    }

    if (setNumberOfDigits == 0 || len == 0 || pf == NULL)
        return 0;


    return dfsNonTrivialCount(pf->trev, len, setNumberOfDigits, setDigits, 0);
}
