# Úkol 7: Testování

## Pohádka začátkem

Slyšel jsem, že studenti mají u úloh rádi pohádky, takže si dáme jednu i tady...

Jste v zaměstnání a vaším úkolem je vytvářet automatické testy, popřípadě z jejich výstupů i kod opravit. 
DostalI jste sadu interfacu, které obsahují definice, co třídy musí umět včetně zadání 
(složka `src/Interfaces`) a k ní i implementace dodané novým juniorním zaměstnancem 
(složka `src/Lib`). 

## Zadání úlohy

### Automatické testy
Vaším úkolem je nejdříve napsat automatické testy, které robustně otestují řešení nováčka.
Pro testy jsou již v repozitáři připraveny třídy ve složce `src/Tests` (**NEPLÉST SE SLOŽKOU `Tests` obsahující testy pro ohodnocení úlohy**).
Testy by měli být schopné otestovat jakékoliv řešení a kontrolovat, že dělá přesně co, co je žádáno v interfacu).

Z důvodu auto testování je nutné místo vytváření vlastní instance testované třídy pomocí `new` nutné 
použít factory třídu ze složky `src/Factory`. Factory se nechovají jako DI, proto vám 
pokaždé vytvoří novou instanci třídy při zavolání metody `get`. Metoda `get` předává všechny přijaté parametry do konstruktoru třídy. 
Použití je naznačeno v testovacích třídách ve složce `src/Tests`.

### Oprava kódu
Po té, co vaše testy objeví chyby, je nutné tyto chyby opravit a to tak, aby prošli nejen vašimi testy, ale odpovídali zadání v příslušném interfacu. Je však i možné že třída je bezchybná, takže testy si vytvořte rozumně.

## Popis implementovaných tříd

### MathUtils

Třída `MathUtils` obsahuje několik matematických funkcí, které mohou obsahovat chyby. Ve tříde `Tests\MathUtilsTest` napište testy, které ověří správnou funkcionalitu. V případě nalezení chyby v implementaci ji opravte. Nezapomeňte otestovat všechny případy.

### UserService

Třída `UserService` ukládá uživatele a načítá o nich data. Používá k tomu interface `Storage`, kde implementace není známá a který se předává do konstruktoru. Napište testy, které ověří funkcionalitu `UserService`, vytvořte mock pro `Storage`, který v testech použijete.

### LinkedList, LinkedListItem

Třída `LinkedList` představuje obousměrně zřetězený seznam položek `LinkedListItem`. Instance `LinkedList` obsahuje referenci na první a poslední prvek, instance `LinkedListItem` na předchozí a následující.
Metody pro vkládání prvků ve třídě `LinkedList` mohou obsahovat chyby. Ve tříde `Tests\LinkedListItemTest` napište testy, které ověří správnou funkcionalitu. V případě nalezení chyby v implementaci ji opravte. Nezapomeňte otestovat všechny případy - spojitost seznamu v obou směrech, správné pořadí položek atd.


## Automatické bodování
* Povinný veřejný test [1,2 bodu]
  * Testuje, zda code coverage implementace je 100%
  * Testuje, zda vámi opravene řešení projde vašimi testy na 100%
* Test implementace [1,2 bodu]
  * Testuje, zda vámi opravená implementace je dle našich testů správně.
* Test MathUtils [1,2 bodu]
  * Testuje robustnost vašich testů pro MathUtils
* Test LinkedList [1,2 bodu]
  * Testuje robustnost vašich testů pro LinkedList
* Test UserService [1,2 bodu]
  * Testuje robustnost vašich testů pro UserService


## Spuštění testů

Pro spuštění vašich testů můžete použít
```shell
composer test-student
```

Pro spuštění veřejného testu ověřující code coverage a funkčost řešení vůči vašim testům
```shell
composer test
```

## Poznámky k úloze

Nedodržení následujícíh poznámek povede ke snížení bodového hodnocení.

* V této úloze není dovoleno použití externích knihoven (vyjma PHPUnit).
* Je nutné mít nainstalováno rozšíření pro PHP, které umožní provést analýzu code coverage pro úspěšné spuštění veřejného testu. (viz. cvičení)
* Máte dovoleno upravovat soubory pouze ve složce `src`. Nezakládejte žádné nové soubory a jiné nemažte.
* Není dovoleno zakládat nebo mazat soubory v této úloze
* Není dovoleno upravovat soubory ve složkách `src/Factory` a `src/Interfaces`
* Je nutné při vytváření instací používat příslušné factory třídy
* Při kontrole Linked listu myslete na to, že špatná implementace může způsobit zacyklední testu. Testy by proto měli obsahovat failsafe, aby k zacyklení nedošlo a vyhodili v tomto případě chybu.
* Vaše testy nesmí být `final`

Eviduji, že podmínky jsou trochu zkličující, ovšem testy na testy jsou celkem robustní záležitost...