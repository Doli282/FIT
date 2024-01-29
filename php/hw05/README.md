# Úkol 5: Faktury

Cílem je vytvořit program, který bude generovat faktury jako PDF dokumenty. 
Máte připravenou kostru aplikace, kde najdete třídy reprezentující věci vyskytující se na faktuře 
a třídu `Builder`, která slouží k vytvoření faktury. Do projektu doinstalujte pomocí composeru 
knihovnu [DomPDF](https://github.com/dompdf/dompdf). Pokud z nějakého důvodu chcete použít 
knihovnu jinou, po domluvě s Otto Šlegerem to možné je. Povolení knihovny by poté bylo umožněno pouze nad 
vaším repozitářem. Žádost si podejte včas. Reakční doba číní 2 dny (+- 2 dny). 

## Požadavky na řešení
1) Vytvořte soubor `composer.json`. Ten bude obsahovat konfiguraci projektu, autoloading ve složce src a požadavky na instalování knihoven.
   * Požadované knihovny
     * [DomPDF](https://github.com/dompdf/dompdf) (verze `2.0.3`)
     * PhpUnit (verze `10.4.1`)
   * Zde se můžete inspirovat u předchozích domácích úkolů
   * Nepřesněrovávejte instalaci do jiné složky, než je ta výchozí (`vendor`)
   * **[1 bod]**
1) Doplňte chybějící části tříd `Invoice\Item`, `Builder` a `Invoice`.
   * Ostatní metody neupravujte, mohlo by dojít na základě úpravy ke zhoršenému hodnocení.
   * **[1 bod]**

1) Implementujte třídu `Renderer`, tak aby vytvořila Html kod faktury (metoda `makeHtml`) a finální PDF (metoda `makePdf`).
   * Test ověřuje, zda faktura obsahuje všchny údaje a metoda makePdf vytváří validní PDF
   * **[1 bod]**

1) Prvky vytvořené faktury
   - Formátování cen na dvě desetinná místa, s desetinnou čárkou a tisíce oddělené mezerou
   - Dodavatel a odběratel s příslušným formátováním (telefonní číslo a email se zobrazí pouze pokud jsou nastavené)
   * **[1 bod]**
   
1) Podoba PDF
    * Finální podobu PDF nemůžeme ověřit automatickými testy, proto se na vaše finální PDF podíváme.
    * Cílem je, aby se vaše PDF co nejvíce přiblížilo podobě v souboru `template.pdf`
    * *Soubor template.pdf nemá cenu upravovat. Máme vlastní zálohu a nebojíme se ji pro hodnocení použít ;)*
    * K manuální kontrole dojde až `18. 11. 2023` (tzn. po deadline)
    * **[2 body]**

## Testování

Svůj kod můžete otestovat pomocí přiloženého scriptu `run.php`

```
$ php run.php > invoice.pdf
```
Pokud vám PDF nejde otevřít, a nevidíte ani žádnou chybu, zkuste provést spuštění programu bez přesměrování výstupu - pravděpodobně se vypisuje chyba, ale namísto na STDOUT jde právě do souboru invoice.pdf
```
$ php run.php
```

Soubor `template.pdf` ukazuje, jak by výsledná faktura měla vypadat. Vaším úkolem je použít knihovnu DomPDF tak, abyste se co nejvíc přiblížili ukázce. Bude potřeba se podívat do dokumentace knihovny, abyste zjistili, jak ji použít. Není potřeba řešit podporu českých znaků.

## Poznámky k úloze
* Body z posledního bodu zadání jsou přidělovány ručně, proto se neděste, že hodnotíci script vám přidělil maximálně 4 body. Zbylé 2 budou dodány ručně.
* V této úloze je možné používat externí knihovny, pokud jsou instalovány pomocí composeru. Pokud chcete, můžete využít váš oblíbený šablonovací systém (inspirace třeba [zde](https://ourcodeworld.com/articles/read/847/top-7-best-open-source-php-template-engines)), nebo se naučit nový. Pro plné splnění zadání to ale není nutné.
* V repozitáři se nesmí nalézat žádné instalované knohovny. K jejich instalaci musí dojít až spuštění composer install.
* Doporučujeme v repozitáři ponechat soubor `composer.lock`. V případě jeho nepřítomnosti může dojít k instalaci jiné verze knihovny, než nad kterou jste váš kod napsali, a u nás to nemusí fungovat.
