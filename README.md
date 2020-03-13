# IPP_zadanie2
Zadanie 2 z przedmiotu 'Indywidualny Projekt Programistyczny'

Treść zadania:

Duże zadanie, część 1
Zadanie drogi, część 1

Tegoroczne duże zadanie polega na zaimplementowaniu obsługi map dróg krajowych. Na potrzeby tego zadania przyjmujemy następujące definicje.

Mapa dróg jest zbiorem miast (ang. city) połączonych odcinkami dróg (ang. road).

Miasto reprezentowane jest przez jego nazwę, która jest niepustym napisem w stylu C niezawierającym kodów od 0 do 31 ani średnika i zakończonym zerem.

Każdy odcinek drogi łączy dwa różne miasta. Między parą miast może być co najwyżej jeden odcinek drogi. Odcinek drogi ma dwa atrybuty: długość, która jest dodatnią liczbą całkowitą i rok, który jest rokiem budowy lub rokiem ostatniego remontu i jest liczbą całkowitą. Wartości dodatnie reprezentują lata n.e., a wartości ujemne to lata p.n.e. Uwaga: nie było roku 0.

Droga jest to ciąg odcinków drogowych łączących dwa różne miasta, bez przerw, samoprzecięć i pętli.

Droga krajowa (ang. route) jest to droga. Jeden odcinek drogi może należeć do wielu dróg krajowych. Droga krajowa identyfikowana jest przez jej numer, który jest liczbą całkowitą z przedziału od 1 do 999. Długość drogi krajowej to suma długości jej odcinków.

Jako pierwszą część zadania należy zaimplementować moduł operacji na mapach drogowych. Opis interfejsu modułu znajduje się w pliku src/map.h w formacie komentarzy dla programu doxygen. Przykład użycia znajduje się w pliku src/map_main.c.
Dostarczamy

W repozytorium https://git.mimuw.edu.pl/IPP-login.git (gdzie login to identyfikator używany do logowania się w laboratorium komputerowym) znajduje się szablon implementacji rozwiązania tego zadania. Znajdują się tam następujące pliki:

    src/map.h – deklaracja interfejsu modułu wraz z jego dokumentacją w formacie doxygen,
    src/map_main.c – przykład użycia modułu,
    CMakeLists.txt – plik konfiguracyjny programu cmake,
    Doxyfile.in – plik konfiguracyjny programu doxygen,
    MainPage.dox – strona główna dokumentacji w formacie doxygen.

Zastrzegamy sobie możliwość nanoszenia poprawek do tego szablonu. Będziemy je umieszczać w gałęzi template/part1. Lista poprawek:

    na razie nie ma żadnych poprawek.

Wymagamy

Jako rozwiązanie części 1 zadania wymagamy:

    uzupełnienia implementacji w pliku src/map.h,
    stworzenia pliku src/map.c z implementacją modułu,
    uzupełnienia dokumentacji w formacie doxygen tak, aby była przydatna dla programistów rozwijających moduł.

Powinna być możliwość skompilowania rozwiązania w dwóch wersjach: release i debug. Wersję release kompiluje się za pomocą sekwencji poleceń:

mkdir release
cd release
cmake ..
make
make doc

Wersję debug kompiluje się za pomocą sekwencji poleceń:

mkdir debug
cd debug
cmake -D CMAKE_BUILD_TYPE=Debug ..
make
make doc

W wyniku kompilacji odpowiednio w katalogu release lub debug powinien powstać plik wykonywalny map oraz dokumentacja. W poleceniu cmake powinno być również możliwe jawne określenie wariantu release budowania pliku wynikowego:

cmake -D CMAKE_BUILD_TYPE=Release ..

Zawartość dostarczonych przez nas plików można modyfikować, o ile nie zmienia to interfejsu modułu i zachowuje wymagania podane w treści zadania, przy czym nie wolno usuwać opcji kompilacji: -std=c11 -Wall -Wextra. Zmiany mogą dotyczyć np. stylu, dokumentacji, deklaracji typedef, włączania plików nagłówkowych, implementacji funkcji jako static inline. Inne pliki źródłowe będące częścią rozwiązania można umieścić w katalogu src. Funkcja main programu musi znajdować się w pliku src/map_main.c, ale zawartość tego pliku nie będzie oceniana w tej części zadania.

____

Duże zadanie, część 2
Zadanie drogi, część 2

Jako drugą część dużego zadania należy zaimplementować program, który, korzystając z modułu zaimplementowanego w części pierwszej, udostępnia operacje na mapie dróg. Program obsługuje tylko jedną mapę dróg. Ponadto należy zaimplementować skrypt w bashu.

Interfejs tekstowy

Program czyta dane ze standardowego wejścia, wyniki wypisuje na standardowe wyjście, a informacje o błędach na standardowe wyjście diagnostyczne.
Dane wejściowe

Program akceptuje następujące polecenia.

numer drogi krajowej;nazwa miasta;długość odcinka drogi;rok budowy lub ostatniego remontu;nazwa miasta;długość odcinka drogi;rok budowy lub ostatniego remontu;nazwa miasta;…;nazwa miasta

Format polecenia jest taki sam, jak w wyniku funkcji getRouteDescription. To polecenie tworzy drogę krajową o podanym numerze i przebiegu. Jeśli jakieś miasto lub odcinek drogi nie istnieje, to go tworzy. Jeśli odcinek drogi już istnieje, ale ma wcześniejszy rok budowy lub ostatniego remontu, to modyfikuje ten atrybut odcinka drogi. Za błąd uznajemy, jeśli odcinek drogi już istnieje, ale ma inną długość albo późniejszy rok budowy lub ostatniego remontu. To polecenie niczego nie wypisuje na standardowe wyjście.

addRoad;city1;city2;length;builtYear

Wywołuje na mapie dróg funkcję addRoad z podanymi parametrami. Niczego nie wypisuje na standardowe wyjście.

repairRoad;city1;city2;repairYear

Wywołuje na mapie dróg funkcję repairRoad z podanymi parametrami. Niczego nie wypisuje na standardowe wyjście.

getRouteDescription;routeId

Wywołuje na mapie dróg funkcję getRouteDescription z podanym parametrem. Jeśli wynik działania tej funkcji jest inny niż NULL, to wypisuje na standardowe wyjście jedną linię z wynikiem działania tej funkcji.

Każde polecenie znajduje się w osobnej linii. Puste linie i linie zaczynające się znakiem # należy ignorować. W poleceniach nazwa miasta jest niepustym napisem niezawierającym kodów od 0 do 31 ani średnika, liczby są zapisywane przy podstawie 10. Spacje w nazwie miasta są istotne.

Obsługa błędów

Jeśli polecenie jest niepoprawne składniowo lub jego wykonanie zakończyło się błędem, czyli odpowiednia funkcja zakończyła się wynikiem false lub NULL, to wypisuje na standardowe wyjście diagnostyczne jednoliniowy komunikat

ERROR n

gdzie n jest numerem linii w danych wejściowych zawierającym to polecenie. Linie numerujemy od jedynki i uwzględniamy ignorowane linie.

Zakończenie działania programu

Program po przetworzeniu wszystkich danych wejściowych powinien zakończyć się kodem wyjścia 0 (ang. exit code) i powinien zwolnić całą zaalokowaną pamięć.

Skrypt

Należy napisać skrypt, którego pierwszy parametr wskazuje (nazwa poprzedzona opcjonalnie ścieżką) na plik z wynikami działania funkcji getRouteDescription, każdy wynik w osobnej linii. Kolejne parametry (przynajmniej jeden) to numery dróg krajowych.

Skrypt dla każdego podanego w argumentach numeru drogi krajowej po kolei szuka w podanym pliku informacji o tej drodze krajowej, wylicza jej długość i wypisuje jedną linię w formacie:

numer drogi krajowej;długość

Jeśli w pliku nie ma informacji o żądanej drodze krajowej, to skrypt nic dla niej nie wypisuje.

Zakładamy, że zawartość pliku wskazanego pierwszym parametrem skryptu jest poprawna. W szczególności dla danego numeru drogi krajowej jest w nim co najwyżej jedna o niej informacja. Natomiast skrypt powinien jak najdokładniej sprawdzać poprawność parametrów i jeśli jest ich za mało lub któryś z nich jest niepoprawny, to powinien zakończyć się kodem wyjścia 1 (ang. exit code). Jeśli parametry są poprawne, skrypt powinien zakończyć się kodem wyjścia 0.

Dostarczamy

Nie dostarczamy żadnego kodu źródłowego. Rozwiązanie drugiej części zadania powinno korzystać z własnego, ewentualnie samodzielnie zmodyfikowanego, rozwiązania części pierwszej.
