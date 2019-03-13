#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "phone_forward.h"
#include "phone_forward.h"

#define numberOfDigits 12
#define zero '0'

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

/** @brief Sprawdza, czy @p s wskazuje na poprawny identyfikator.
 *
 * @param[in] s  – wskaźnik na ciąg znaków.
 * @return Wartość @p true, jeżeli @p s wskazuje na poprawny numer.
 *         Wartośc @p false w przeciwnym wypadku.
 */

static bool is_id(char const *s)
{
    if (s == NULL)
        return false;

    size_t len = strlen(s);

    if (len > 0)
    {
        if (s[0] < 'A' || s[0] > 'z')
            return false;

    }

    for(unsigned int i = 0; i < len; i++)
    {
        if (!((s[i] >= zero && s[i] < (zero + 10)) || (s[i] >= 'A' && s[i] <= 'z')))
            return false;
    }

    return (len > 0);
}

char c; ///< zmienna, która służy do wczytywania
bool eof = false; ///< zmienna, która wskazuje, czy pojawił się już EOF

/// Czyta bajt z wejścia do zmiennej @c, jeżeli na wejściu jest eof to zmienia stan @eof
void readChar()
{
    eof = (scanf("%c", &c) == EOF);
}


/** @brief Wypisuje numery z @p pnum.
 *
 * @param[in] pnum  – wskaźnik na strukturę przechowującą numery telefonów.
 */
void printPhoneNumbers(struct PhoneNumbers *pnum)
{
    for(int i = 0; i < pnum->size; i++)
    {
        printf("%s\n", pnum->tab[i]);
    }
}

/** @brief Kopiuje łańcuch znaków.
 * Tworzy nowy łańcuch znaków, który jest taki sam jak ten wskazany przez @p s.
 *
 * @param[in] s – wskaźnik na łańcuch znaków.
 * @return Wskaźnik na skopiowany łańcuch znaków.
 */
static char const *copy_string(char const *s)
{
    size_t len = strlen(s);

    char *new_s = malloc(sizeof(char const) * len + 1);

    new_s = strcpy(new_s, s);

    return new_s;
}

// parser

/**
 * Struktura przedstawiająca bazę przekierowań.
 */
struct ForwardingBase
{
    const char *name; ///< nazwa bazy
    struct PhoneForward *phoneFor; ///< struktura przechowująca przekierowania
};

/** @brief Tworzy nową strukturę.
 * Tworzy nową strukturę niezawierającą żadnych przekierowań o nazwie @p name.
 *
 * @return Wskaźnik na utworzoną strukturę lub NULL, gdy nie udało się
 *         zaalokować pamięci.
 */
struct ForwardingBase *forBaseNew(const char *name)
{
    struct ForwardingBase *b = (struct ForwardingBase*)malloc(sizeof(struct ForwardingBase));

    if (b != NULL)
    {
        b->name = copy_string((char const*)name);
        b->phoneFor = phfwdNew();
    }

    return b;
}

/**
 * Struktura przechowująca bazy przekierowań.
 */
struct Bases
{
    int size; ///< rozmiar struktury (ile jest aktualnie baz w strukturze)
    struct ForwardingBase *bases[101]; ///< tablica, w której przechowuje wskaźniki na strukturę
};

/** @brief Tworzy nową strukturę.
 * Tworzy nową strukturę niezawierającą żadnych baz.
 *
 * @return Wskaźnik na utworzoną strukturę lub NULL, gdy nie udało się
 *         zaalokować pamięci.
 */
struct Bases *basesNew()
{
    struct Bases *b = (struct Bases*)malloc(sizeof(struct Bases));

    if (b != NULL)
        b->size = 0;

    return b;
}

long long bytesCounter = 0; ///< licznik wczytanych bajtów
long long previousBytesCounterState = 1; ///< pomocnicza zmienna do liczenia, który to bajt
char buffer[100003]; ///< bufor, do którego wczytuję wejście
int idx = 0; ///< indeks bufora
struct ForwardingBase *actualBase = NULL; ///< wskaźnik na aktualną bazę
struct Bases *bas; ///< wskaźnik na stukturę przechowującą bazy przekierowań

/** @brief Dodaje nową bazę przekierowań.
 * Dodaje nową bazę przekierowań do struktury, na którą wskazuje @p b.
 *
 * @param[in] b   – wskaźnik na strukturę przechowującą bazy przekierowań;
 * @param[in] name – wskaźnik na napis reprezentujący nazmę bazy;
 *
 * @return Wskaźnik na utworzoną bazę przekierowań lub NULL,
 *         jeżeli nie udało się zaalokować pamięci
 */
struct ForwardingBase* addBase(struct Bases *b, const char *name)
{
    b->bases[b->size] = forBaseNew(name);
    b->size++;

    return b->bases[b->size - 1];
}

/** @brief Dealokuje bazę przekierowań.
 * Dealokuje bazę przekierowań, która jest wskazywana przez @p b.
 *
 * @param[in] b   – wskaźnik na bazę przekierowań;
 */
void baseDel(struct ForwardingBase *b)
{
    free((char*)b->name);
    phfwdDelete(b->phoneFor);
    free(b);
}

/** @brief Usuwa bazę przekierowań ze struktury.
 * Usuwa bazę przekierowań o nazwie @p name ze struktury, na którą wskazuje @p b.
 *
 * @param[in] b   – wskaźnik na strukturę przechowującą bazy przekierowań;
 * @param[in] name – wskaźnik na napis reprezentujący nazmę bazy;
 *
 * @return Wartość @p true, jeśli baza przekierowań o nazwie @p name została usunieta ze struktury
 *         Wartość @p false, jeśli nie było bazy o nazwie @p name w strukturze.
 */
bool removeBase(struct Bases *b, const char *name)
{
    for(int i = 0; i < b->size; i++)
    {
        if (strcmp(b->bases[i]->name, name) == 0)
        {
            if (b->bases[i] == actualBase)
                actualBase = NULL;

            baseDel(b->bases[i]);
            b->bases[i] = b->bases[b->size - 1];
            b->size--;
            return true;
        }
    }

    return false;
}

/** @brief Usuwa strukturą przechowującą bazy przekierowań.
 * Usuwa strukturą przechowującą bazy przekierowań, na którą wskazuje @p b .
 *
 * @param[in] b   – wskaźnik na strukturę przechowującą bazy przekierowań.
 */
void delBases(struct Bases *b)
{
    for(int i = 0; i < b->size; i++)
    {
        baseDel(b->bases[i]);
    }
    free(b);
}

/** @brief Zwraca wskaźnik na bazę przekierowań ze struktury wskazywanej prze @p b .
 * Zwraca wskaźnik na bazę przekierowań ze struktury wskazywanej przez @p b , której
 * nazywa to łańcuch znaków wskazywany przez @p name .
 *
 * @param[in] b   – wskaźnik na strukturę przechowującą bazy przekierowań;
 * @param[in] name – wskaźnik na napis reprezentujący nazmę bazy.
 *
 * @return Wskaźnik na bazę o nazwie wskazywanej prze @p name lub NULL, jeżeli
 *         nie istnieje taka baza w strukturze wskazanaej przez @p b .
 */
struct ForwardingBase* getForwardingBase(struct Bases *b, const char *name)
{
    for(int i = 0; i < b->size; i++)
    {
        if (strcmp(b->bases[i]->name, name) == 0)
            return b->bases[i];
    }

    return NULL;
}

/** @brief Dodaje przekierowanie do aktualnej bazy.
 * Dodaje przekierowanie do aktualnej bazy z numeru wskazywanego przez @p num1
 * na numer wskazywany przez @p num2.
 *
 * @param[in] num1   – wskaźnik na numer;
 * @param[in] num2 – wskaźnik na numer;
 *
 * @return Wartość @p true, jeśli udało się przekierować.
 *         Wartość @p false w przeciwnym wypadku.
 */
bool addForward(char const *num1, char const *num2)
{
    return phfwdAdd(actualBase->phoneFor, num1, num2);
}

/** @brief Wyświetla przekierowania na dany numer @p num z aktualnej bazy.
 * Wyświetla przekierowania na dany numer @p num z aktualnej bazy.
 *
 * @param[in] num   – wskaźnik na numer;
 *
 * @return Wartość @p true, jeśli udało się wypisać numer.
 *         Wartość @p false w przeciwnym wypadku lub jeżeli nie było przekierowania z @p num.
 */
bool printForwardFromNum(char const *num)
{
    const struct PhoneNumbers* phnum  = phfwdGet(actualBase->phoneFor, num);

    if (phnum->size == 0)
        return false;

    printPhoneNumbers((struct PhoneNumbers*)phnum);
    phnumDelete(phnum);

    return true;
}

/** @brief Wyświetla przekierowania na dany numer @p num z aktualnej bazy.
 * Wyświetla przekierowania na dany numer @p num z aktualnej bazy.
 *
 * @param[in] num   – wskaźnik na numer;
 *
 * @return Wartość @p true, jeśli udało się wypisać numer.
 *         Wartość @p false w przeciwnym wypadku lub jeżeli nie było przekierowania na @p num.
 */

bool printForwardToNum(char const *num)
{
    const struct PhoneNumbers* phnum  = phfwdReverse(actualBase->phoneFor, num);

    if (phnum->size == 0)
        return false;

    printPhoneNumbers((struct PhoneNumbers*)phnum);
    phnumDelete(phnum);

    return true;
}

/** @brief Usuwa przekierowania.
 * Usuwa z aktualnej bazy wszystkie przekierowania, w których parametr @p num jest prefiksem
 * parametru @p num1 użytego przy dodawaniu. Jeśli nie ma takich przekierowań
 * lub napis nie reprezentuje numeru, nic nie robi.
 *
 * @param[in] num – wskaźnik na napis reprezentujący prefiks numerów.
 */
void removeForwards(char const *num)
{
    phfwdRemove(actualBase->phoneFor, num);
}

/** @brief Wypisuje wynik funkcji phfwdNonTrivialCount.
 * Wypisuje wynik funkcji phfwdNonTrivialCount z parametrem set jako @p set
 * i parametrem len jako max(0, długość @p set).
 *
 * @param[in] set – wskaźnik na napis, który jest
 *                  zbiorem znaków do wyliczenia liczby nietrywailnych numerów.
 */
void printNumberOfNonTrivials(char const *set)
{
    size_t len = 0;

    if (strlen(set) > 12)
        len = (strlen(set) - 12);

    printf("%lu\n", phfwdNonTrivialCount(actualBase->phoneFor, set, len));
}


/** @brief Sprawdza jaki keyword identyfikuje komendę w buforze.
 * Sprawdza jaki keyword identyfikuje komendę w buforze.
 *
 * @return identyfikator komendy rozpoznanej w buforze lub 0,
 *          jeżeli w buforze nie ma komendy.
 */
int keyword()
{
    if (buffer[0] == '?')
        return 1;
    else if (buffer[0] == '@')
        return 2;
    else if (buffer[idx - 1] == '?')
    {
        buffer[idx - 1] = '\0';
        return 5;
    }
    else if (idx > 2)
    {
        if (buffer[0] == 'N' && buffer[1] == 'E' && buffer[2] == 'W')
            return 3;

        if (buffer[0] == 'D' && buffer[1] == 'E' && buffer[2] == 'L')
            return 4;

        for(int i = 0; i < idx; i++)
        {
            if (buffer[i] == '>')
                return 6;
        }
    }
    else
    {
        bool aux = true;
        for(int i = 0; i < idx; i++)
        {
            if (isspace(buffer[i]) == 0)
            {
                aux = false;
                break;
            }
        }

        if (aux)
            return 7;
    }

    return 0;
}

/** @brief Przetwarza komendę, która znajduje się w buforze.
 * Przetwarza komendę, która znajduje się w buforze.
 * Komenda ta ma identyfikator @p keyword.
 *
 * @param[in] keyword - identyfikator komendy w buforze;
 *
 * @return Wartość @p true, jeśli udało się poprawnie przetworzyć komendę.
 *         Wartość @p false w przeciwnym wypadku.
 */
bool processParticularCommand(int keyword)
{
    switch(keyword)
    {
        case 0:
            {
                fprintf(stderr, "ERROR %lld\n", previousBytesCounterState);
                return false;
            }
        case 1: //? *
            {
                const char *name = copy_string(buffer + 1);

                if (actualBase == NULL)
                {
                    fprintf(stderr, "ERROR ? %lld\n", previousBytesCounterState);
                    free((char*)name);
                    return false;
                }

                if (!is_number(name))
                {
                    fprintf(stderr, "ERROR %lld\n", previousBytesCounterState);
                    free((char*)name);
                    return false;
                }

                if (actualBase != NULL)
                {
                    bool aux = printForwardToNum(name);

                    if (!aux)
                        fprintf(stderr, "ERROR ? %lld\n", previousBytesCounterState);
                }
                else
                {
                    fprintf(stderr, "ERROR ? %lld\n", previousBytesCounterState);
                    free((char*)name);
                    return false;
                }

                free((char*)name);

                return true;
            }
        case 2: //@ *
            {
                const char *name = copy_string(buffer + 1);

                if (actualBase == NULL)
                {
                    fprintf(stderr, "ERROR @ %lld\n", previousBytesCounterState);
                    free((char*)name);
                    return false;
                }

                if (!is_number(name))
                {
                    fprintf(stderr, "ERROR %lld\n", previousBytesCounterState);
                    free((char*)name);
                    return false;
                }

                if (actualBase != NULL)
                    printNumberOfNonTrivials(name);
                else
                {
                    fprintf(stderr, "ERROR @ %lld\n", previousBytesCounterState);
                    free((char*)name);
                    return false;
                }

                free((char*)name);

                return true;
            }
        case 3: // NEW
            {
                const char *name = copy_string(buffer + 3);

                if (!is_id(name) || strcmp("NEW", name) == 0 || strcmp("DEL", name) == 0)
                {
                    fprintf(stderr, "ERROR %lld\n", previousBytesCounterState + 4);
                    free((char*)name);
                    return false;
                }

                actualBase = getForwardingBase(bas, name);

                if (actualBase == NULL)
                    actualBase = addBase(bas, name);

                free((char*)name);

                return true;
            }
        case 4: // DEL
            {
                const char *name = copy_string(buffer + 3);

                if (is_id(name) && strcmp("DEL", name) != 0 && strcmp("NEW", name) != 0)
                {
                    if (!removeBase(bas, name))
                    {
                        fprintf(stderr, "ERROR DEL %lld\n", previousBytesCounterState);
                        free((char*)name);
                        return false;
                    }
                }
                else if (is_number(name))
                {
                    if (actualBase != NULL)
                        removeForwards(name);
                    else
                    {
                        fprintf(stderr, "ERROR DEL %lld\n", previousBytesCounterState);
                        free((char*)name);
                        return false;
                    }
                }
                else
                {
                    fprintf(stderr, "ERROR %lld\n", previousBytesCounterState + 4);
                    free((char*)name);
                    return false;
                }

                free((char*)name);

                return true;
            }
        case 5: // * ?
            {
                const char *name = copy_string(buffer);

                if (actualBase == NULL)
                {
                    fprintf(stderr, "ERROR ? %lld\n", previousBytesCounterState + idx);
                    free((char*)name);
                    return false;
                }

                if (!is_number(name))
                {
                    fprintf(stderr, "ERROR %lld\n", previousBytesCounterState + idx);
                    free((char*)name);
                    return false;
                }

                if (actualBase != NULL)
                {
                    bool aux = printForwardFromNum(name);

                    if (!aux)
                        fprintf(stderr, "ERROR ? %lld\n", previousBytesCounterState + idx);
                }
                else
                {
                    fprintf(stderr, "ERROR ? %lld\n", previousBytesCounterState + idx);
                    free((char*)name);
                    return false;
                }

                free((char*)name);

                return true;
            }
        case 6: // * > *
            {

                int division = 0;
                for(int i = 0; i < idx; i++)
                {
                    if (buffer[i] == '>')
                    {
                        division = i;
                        buffer[i] = '\0';
                        break;
                    }
                }

                const char *num1 = copy_string(buffer);
                const char *num2 = copy_string(buffer + division + 1);

                if (actualBase == NULL)
                {
                    fprintf(stderr, "ERROR > %lld\n", previousBytesCounterState + division + 1);
                    free((char*)num1);
                    free((char*)num2);
                    return false;
                }

                if (!is_number(num1) || !is_number(num2))
                {
                    if (!is_number(num1))
                        fprintf(stderr, "ERROR %lld\n", previousBytesCounterState);
                    else
                        fprintf(stderr, "ERROR %lld\n", previousBytesCounterState + idx - strlen(num2));

                    free((char*)num1);
                    free((char*)num2);
                    return false;
                }

                if (actualBase != NULL)
                {
                    if (!addForward(num1, num2))
                    {
                        fprintf(stderr, "ERROR > %lld\n", previousBytesCounterState + division);
                        free((char*)num1);
                        free((char*)num2);
                        return false;
                    }
                }
                else
                {
                    fprintf(stderr, "ERROR > %lld\n", previousBytesCounterState + division);
                    free((char*)num1);
                    free((char*)num2);
                    return false;
                }

                free((char*)num1);
                free((char*)num2);

                return true;

            }
        case 7: //pusta linia
            {
                return true;
            }
    }

    return false;
}

/** @brief Wczytuje i przetwarza komendę z wejścia.
 * Wczytuje komendę do bufora i przetwarza ją za pomocą
 * funkcji @p processParticularCommand.
 *
 * @return Wartość @p true, jeśli udało się poprawnie przetworzyć komendę.
 *         Wartość @p false w przeciwnym wypadku.
 */

bool processCommand()
{
    readChar();
    bytesCounter++;

    previousBytesCounterState = bytesCounter;

    // Pozbywa się białych znaków z początku
    if (isspace(c) != 0 && !eof && c != '\n')
    {
        while(isspace(c) != 0 && !eof && c != '\n')
        {
            readChar();
            bytesCounter++;
        }
    }

    while(!eof && c != '\n')
    {
        // Pozbywa się białych znaków
        if (isspace(c) != 0 && c != '\n' && !eof)
        {
            while(isspace(c) != 0 && c != '\n' && !eof)
            {
                readChar();
                bytesCounter++;
            }
        }

        // Przetwarza komentarze
        // Jeżeli jest to poprawny komentarz to zastępuje go spacją

        long long commentCounter = bytesCounter;

        if (c == '$')
        {
            readChar();
            bytesCounter++;

            if (c == '$')
            {
                readChar();
                bytesCounter++;

                char d = 'd';

                while((c != '$' || d != '$') && !eof)
                {
                    d = c;
                    readChar();
                    bytesCounter++;
                }

                if (eof)
                {
                    if (idx > 0)
                        fprintf(stderr, "ERROR %lld\n", commentCounter);
                    else
                        fprintf(stderr, "ERROR EOF\n");

                    return false;
                }

            }
            else
            {
                fprintf(stderr, "ERROR %lld\n", bytesCounter - 1);
                return false;
            }

            c = ' ';
        }

        if (!eof)
        {
            buffer[idx] = c;
            idx++;

            readChar();
            bytesCounter++;
        }

    }

    if (idx > 0)
        buffer[idx] = '\0';

    /*printf("buffer: ");
    for(int i = 0; i < idx; i++)
    {
        printf("%c", buffer[i]);
    }
    printf("\n");*/

    return processParticularCommand(keyword());

}

int prevKeyword = 0, newKeyword = 0;

int main()
{
    // Tworzy nową strukturą baz przekierowań
    bas = basesNew();

    while(!eof)
    {
        // jeżeli niepoprawna komenda to kończy program
        if (!processCommand())
        {
            delBases(bas);
            return 1;
        }

        idx = 0;
        buffer[0] = '#';
    }

    //usuwa strukture przechowującą bazy przekierowań
    delBases(bas);
    return 0;
}
