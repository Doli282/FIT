# Úkol 6: Bankovní transakce

Cílem je vytvořit program, který bude importovat bankovní transakce ze souboru do databáze a vypisovat souhrn pro jednotlivé bankovní účty. Uložení do databáze zajistí persistenci dat mezi jednotlivými spuštěními aplikace.

Máte připravenu kostru aplikace a úkolem bude doplnit implementace určených metod.

## Třída `Db`

Obsahuje jedinou statickou metodu `get`, která realizuje a reprezentuje spojení s SQLite databází pomocí PDO.
Pozor!!! Testy probíhájí nad databazi SQLLite, některé konstrukce jako např. Cascade nebudou funkovat. Prosíme počítejte s tím.

## Model `Account`

Obsahuje datový model pro bankovní účet, instance odpovídá řádku v databázi, třída odpovídá tabulce. Atribut `id` je jednoznačný - databází automaticky generovaný - identifikátor; v databázi primární klíč.
Zbylé atributy `number` a `code` jsou číslo účtu resp. kód banky. Tabulka musí se jmenovat `accounts` 
 
## Model `Transaction`

Datový model pro transakci mezi dvěma bankovními účty. Transakce je převod částky `amount` z účtu `account_from` na účet `account_to`.
V PHP jsou atributy `from` a `to` referencemi na objekt `Account`; v databázi bude vazba reprezentovaná pomocí cizího klíče.
Atribut `id` je jednoznačný - databází automaticky generovaný - identifikátor; v databázi primární klíč. Pozor, tabulka se musí jmenovat `transactions`

## `data`
V adresáři `data` naleznete dva soubory s transakcemi ve formátu
```
cislo_uctu kod_banky cislo_uctu kod_banky castka
``` 

## Implementujte

1. autoloadování aplikace skrze `composer.json`

1. statické metody `Account::createTable`, `Transaction::createTable`, `Account::dropTable`, `Transaction::dropTable` - metody slouží k inicializaci (resetování) databáze. Použijte volání odpovídajícího SQL příkazu.

1. statickou metodu `Account::find` - která vrátí (vytvoří) instanci `Account` pro odpovídající řádek  z databázové tabulky. V případě nenalezení řádku vrátí `null`.

1. statickou metodu `Account::findById` - která vrátí (vytvoří) instanci `Account` pro odpovídající primírní klíč (id) z databázové tabulky. V případě nenalezení řádku vrátí `null`.

1. statickou metodu `Account::findOrCreate` - která vrátí (vytvoří) instanci podle existujícího řádku nebo podle nově vloženého.

1. instanční metodu `$account->getTransactions` - která vrátí pole instancí `Transaction` související s účtem (převod z/na daný účet).

1. instanční metodu `$account->getTransactionSum` - která spočítá aktuální stav účtu (tzn. pro transakce z metody výše) pomocí databázové agregační funkce. Odchozí transakce odečítá, příchozí přičítá.

1. instanční metodu `$transaction->insert`, která vloží data aktuální instance do databáze.

Neupravujte ostatní soubory.

## Ověření implementace

Aplikace se používá pomocí skriptu `run.php`, který přijímá argumenty z příkazové řádky.
- Příkaz `init` slouží k inicializaci (resetování) stavu databáze - odstraní a znovu vytvoří tabulky.
- Příkaz `import <soubor>` importuje transakce ze souboru
- Příkaz `summary <cislo uctu> <kod banky>` zobrazí transakce a součet pro zadaný účet

Vaší implementaci můžete ověřit pomocí shellového skriptu `test.sh`, který porovná očekávané výstupy s výstupy vaší implementace. Skript zobrazuje diff Vašeho a očekávaného výstupu. Pokud je program zcela správně, mělo by na výstupu být:

```
== init
== import tr1
== summaries
Files output/111300_0710_1.txt and - are identical
Files output/111323_0710_1.txt and - are identical
Files output/330330_0800_1.txt and - are identical
Files output/330331_0800_1.txt and - are identical
== import tr2
== summaries
Files output/100100_0300_2.txt and - are identical
Files output/100100_0710_2.txt and - are identical
Files output/101010_2010_2.txt and - are identical
Files output/111300_0710_2.txt and - are identical
Files output/111323_0710_2.txt and - are identical
Files output/330330_0800_2.txt and - are identical
Files output/330331_0800_2.txt and - are identical
Files output/330335_0800_2.txt and - are identical
```

## Setup automatických testu

Máte na výběr 2 možnosti, bud to spustit lokalně => pak musíte mít instalovano php a composer a nebo použít připravený docker-compose.yaml.

Postup pro vývoj na lokalu:

1. Musíte si instalovat php, composer a sqlLite a nebo postgres databázi, POZOR pro testování používáme SQL lite db, pozor na rozdíly v syntaxe,
pro každý operačný systém instalace je jiná, zkuste si vyhledat návod na webu.
2. Instalace composer pro ubuntu: `sudo apt-get install composer`
3. Pote musíte instalovat knihovny pomocí příkazu: `composer install`
4. Dále můžete spouštět lokalní testování pomocí: `php run.php`
5. Přípravené testy můžete pustit pomocí příkazu: `composer test`
6. Přípojení k db si můžete změnit v souboru `src\Db.php`

Postup pro docker:

1. V kořenu projektu nalezněte docker-compose.yaml soubor
2. Příhlašte se do registry pomocí docker login gitlab.fit.cvut.cz:5050 -u <username> -p <access_token> pokud jste to ještě neudělali.
3. Zavolejte přikaz v konzoli: `docker-compose up`
4. Otevřete novou založku a spuste příkaz `docker-compose exec php bash`, pomocí kterého se připojíte k běžicímu php kontejneru.
5. Pote musíte instalovat knihovny pomocí příkazu: `composer install`
6. Přípojení k db si můžete změnit v souboru `src\Db.php` -> výchozí připojení je nastaveno na SQLLite a pro testování také používáme SQL Lite, Pozor na to.
7. Můžete si nastavit připojení k db v PHP stormu(postgres):
   * host: localhost, 
   * port: 5432
   * username: dev, 
   * password: pass
8. Pro Sql Lite stačí zvolit soubor s touto db po prvním volání. Soubor se objeví v kořenu projektu.
9. Dále můžete spouštět lokalní testování pomocí: `php run.php`
10. Přípravené testy můžete pustit pomocí příkazu: `composer test`

Pozor pokud používate docker, všechny tyto přikazy musíte volat uvnitř kontejneru a nebo nastavit si remote interpretor v php stormu.
