# Úkol 9: Web scraping

Vaším úkolem je vytvořit scraper produktů z e-shopů: https://bi-php.urbanec.cz

Víte, že váš kamarád slaví narozeniny o Vánocích, a proto chcete vybrat pro něj hezký dárek. Rozhodli jste se mu koupit nový mobilní telefon. Vzhledem k aktuálnímu Black Friday jsou některé zboží pravděpodobně zlevněné, a váš rozpočet na tento dárek je maximálně 25 000 Kč.

Vaším úkolem je vytvořit scraping aplikaci, která sestaví seznam mobilních telefonů, ze kterých byste mohli vybrat dárek. Tento seznam by měl obsahovat ceny, popisy, hodnocení a komentáře uživatelů. Hledání můžete provést zadáním řetězce `phone` do pole vyhledávání, které vám umožní filtrovat pouze telefony. Mějte na paměti, že tento filtr může vyhledat i jiné zboží než mobilní telefony, a tuto nežádoucí položku byste měli odstranit ze seznamu.



- Každá položka v seznamu by měla obsahovat následující informace: 
  - název,
  - cenu,
  - Pokud je product zlevněn, tak i cenu před zlevněním. Pokud product zlevněn nebyl tak bude null.
  - popis (musí být ze stránky s detailem)
  - odkaz na produkt (detail)
  - Hodnocení uživatelů (seznam: uživatelské jméno: komentář: hodnocení) Instance třídy RatingResult
  - Získané (relevantní) položky ze všech e-shopů zobrazte seřazené podle ceny vzestupně.

Máte implementované pomocné třídy ProductResult a RatingResult, do kterých uložté výsledek.

Implementujte metodu run() a další pomocné metody ve třidě App

## Postup

- Použijte knihovnu Symfony/Crawler a HttpBrowser nebo obdobnou.
- Vytvořte HTTP Request jehož výsledkem bude výsledek vyhledávání podle výrazu
- Iterujte přes jednotlivé produkty a pro každý získejte stránku s detailem
- Ze stránky s detailem extrahujte a uložte požadované údaje
- Z uložených údajů odstraňte irrelevantní produkty (např. podle názvu produktu nebo slov v popisu)
- Sestavte přehledný výpis ze získaných údajů seřazený podle ceny vzestupně 
