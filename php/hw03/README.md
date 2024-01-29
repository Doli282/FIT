# Úkol 3 - Třídy

## 1. Třída Bag

Implementujte třídu Bag, která se nachází v `classes/Bag.php`. Třída má 7 metod:

- `add($item)` - přidá prvek do kolekce, nevrací nic
- `clear()` - vyprázdní kolekci
- `contains($item)` - vrací `true` pokud se prvek nachází v kolekci, `false` v opačném případe
- `elementSize($item)` - vrací počet výskytů daného prvku
- `isEmpty()` - vrací `true` pokud je kolekce prázdná, `false` v opačném případě
- `remove()` - odebere prvek z kolekce, pokud tam prvek není, nestane se nic. Pokud je tam vícekrát, odebere se pouze jeden výskyt
- `size()` - vrací celkový počet prvků v kolekci


## 2. Autoloader

V souboru `main.php` je místo pro registrování autoloaderu. Implementuje a zaregistrujte autoloader, který zajistí načítání tříd ze složky `classes`. Testováno bude pouze na třídách bez namespaců.

Příklady použití můžete nalézt ve veřejných testech (soubor `tests/public/BasicTest.php`), které můžete využít k lepšímu pochopení zadání.


## 3. Třída Set

Vytvoře novou třídu `Set` jako potomka třídy `Bag`, která se chová následovně:

- `add($item)` - přidá prvek do kolekce, v případě, že už se tam prvek nachází, nestane se nic
- `clear()` - vyprázdní kolekci
- `contains($item)` - vrací `true` pokud se prvek nachází v kolekci, `false` v opačném případe
- `elementSize($item)` - vrací počet výskytů daného prvku, v případě `Set` je to 0 nebo 1
- `isEmpty()` - vrací `true` pokud je kolekce prázdná, `false` v opačném případě
- `remove()` - odebere prvek z kolekce
- `size()` - vrací celkový počet prvků v kolekci

Funkcionalita se částečně shoduje s třídou `Bag`, **snažte se tedy neimplementovat vše znovu, ale využít vlastností OOP ke sdílení funkcionality.** - nedodržení tohoto požadavku může vést ke stržení bodů mimo automatické testování.


## 4. Kontrola správnosti

Až budete mít vše implementované, zkuste si spustit veřejné testy. 

K tomu je potřeba mít nainstalovaný v systému composer:
```shell
sudo apt-get install composer
```

Testy využívají knihovnu PHP-Unit, kterou potřebujeme do projektu naimportovat (její soubory nejsou v projektu a na gitu k dispozici). To uděláme pomocí:

```shell
composer install
```

Pro následné spuštění testů pak stačí:
```shell
composer test
```

## Poznámky k úloze
- V této úloze nejsou dovoleny žádné externí knihovny ani kódy z internetu. Php unit slouží pouze pro testovací účely.
- Neuproavujte rozhraní předpřipravených souborů, jejich úprava by mohla narušit testování, což by vedlo k 0 bodům.
- Obsah složky `tests` neupravujte ani nemažte. Odstranění bude mít za následek ohodnocení úlohy 0 body.
- Třídy `Set` i `Bag` by měli být schopný pracovat se všemi datovými typy. Není zaručeno, že všedchny vložené elementy do `Bag` / `Set` budou sdílet stejný datový typ.
- Emementy různých datových typů jsou automaticky různé elementy. Pokud například bychom do `Set` vložily elementy `"10"` (string) a `10` (int), tak v `Set` budou 2 elementy a né jeden.
- Soubor `tests/public/BasicTest.php` obsahuje povinné testy, na které se můžete podívat. Pokud váš program neprojde těmito testy, bude automaticky ohodnocen 0 body.
