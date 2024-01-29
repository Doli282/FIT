# Úkol 4

Ve složce src je třída `Node`, která reprezentuje uzel binárního stromu. Každý uzel může mít nějakou číselnou hodnotu a levého a pravého potomka. Pokud uzel nemá některého potomka má atribut hodnotu `null` v opačném případě obsahuje referenci na jiný uzel `Node`.

Cílem je vytvořit implementace iterátorů (interface [Iterator](https://www.php.net/manual/en/class.iterator.php)) pro průchod stromu v pořadí

- in-order,
- pre-order a
- post-order.

Iterátor zajistí, že strom lze procházet např. pomocí `foreach`. 

## Zadání

1. Implementujte a zaregistrujte autloader, který zajistí správně načítání tříd, včetně namespaců ze složky `src`. (1 bod)
2. Doplňte metody `Iterator\InOrderIterator` pro **in-order** průchod. (1 bod). `Iterator\AbstractOrderIterator` si můžete upravit, jak budete potřebovat, při zachování `\Iterator` interface.
3. Doplňte metody `Iterator\PreOrderIterator` a `Iterator\PostOrderIterator` pro **pre-order** resp. **post-order** průchod. Snažte se sdílet společný kód prostřednictvím dědičnosti. (2 body) 
4. Upravde třídu `Node`, aby implementovala [IteratorAggregate](https://www.php.net/manual/en/class.iteratoraggregate.php), ve výchozím stavu bude používat `InOrderIterator`. (2 body)
5. Neúpravujte rozhraní předpřipravených souborů, jejich úprava by mohla narušit testování, což by vedlo k 0 bodům. 
6. Obsah složky tests neupravujte ani nemažte. Odstranění bude mít za následek ohodnocení úlohy 0 body.
7. Soubory ve složce tests/public obsahují povinné testy, na které se můžete podívat. Pokud váš program neprojde těmito testy, bude automaticky ohodnocen 0 body.
8. Třida [TreeTestHelper.php](src%2FTester%2FTreeTestHelper.php) slouží pro pomoc s testováním vašeho řešení, nesmíte ji změnit, jinak řešení bude hodnocenu 0 body.
9. Pokud chcete si vytvořit vlástní testy, tak si můžete vytvořit nový soubor ve složce tests, ale nesmíte ho dávat na gitlab.



## Setup

Máte na výběr 2 možnosti, bud to spustit lokalně => pak musíte mít instalovano php a composer a nebo použít připravený docker-compose.yaml.

Postup pro vývoj na lokalu:

1. Musíte si instalovat php a composer, pro každý operačný systém instalace je jiná, zkuste si vyhledat návod na webu.
2. Instalace composer pro ubuntu: `sudo apt-get install composer`
3. Pote musíte instalovat knihovny pomocí příkazu: `composer install`
4. Dále můžete spouštět lokalní testování pomocí: `php run.php`
5. Přípravené testy můžete pustit pomocí příkazu: `composer test`

Postup pro docker:

1. V kořenu projektu nalezněte docker-compose.yaml soubor
2. Příhlašte se do registry pomocí docker login gitlab.fit.cvut.cz:5050 -u <username> -p <access_token> pokud jste to ještě neudělali.
3. Zavolejte přikaz v konzoli: `docker-compose up`
4. Otevřete novou založku a spuste příkaz `docker-compose exec php bash`, pomocí kterého se připojíte k běžicímu php kontejneru.
5. Pote musíte instalovat knihovny pomocí příkazu: `composer install`
6. Dále můžete spouštět lokalní testování pomocí: `php run.php`
7. Přípravené testy můžete pustit pomocí příkazu: `composer test`

Pozor pokud používate docker, všechny tyto přikazy musíte volat uvnitř kontejneru a nebo nastavit si remote interpretor v php stormu.
