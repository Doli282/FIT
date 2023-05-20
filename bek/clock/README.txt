Aplikaci jsem napsal pro unixový operační systém.
Aplikace se skládá z python skriptu 'SH.py', konfiguračního souboru 'SH.config' a binárního souboru 'NC'.

---- SH.py ----
Python skript (hlavní aplikaci) jsem umístil do /usr/bin/, jelikož se jedná o místo, kde jsou i jiné spustitelné soubory. Běžný uživatel se nemá právo zápisu, a já nechci, aby bylo možno skript změnit. Nicméně, skript ponechávám čitelný. (`chmod 0755 /usr/bin/SH.py`)
SH.py běží vícevláknově:
Vedlejší vlákno spustí HTTP server, který odbavuje požadavky vzdálených uživatelů a vrací jim aktuální čas v jimi zadefinovaném formátu. Port, na kterém server poběží si aplikace přečte v konfiguračním souboru.
Pro získávání požadavů od vzdálených uživatelů přijímá server GET reuqesty s hodnotou parametru. Vybraný formátovací řetězec je vložen právě do parametru URL. Na stránce se poté zobrazí výsledek.

Hlavní vlákno je pro lokálního uživatele. Lokální uživatel může také zadat formát, ve kterém se má vypsat aktuální čas. Navíc může přejít do nastavovacího módu příkazem "set". Poté v definovaném formátu zadá nový čas. Tím se spustí aplikace NC, která změnu provede.

Pro validace vstupů a výpisu času dle vstupního formátovacího řetězce od uživatele využívám knihovní funkce pythonu z knihovny datetime - strptime a strftime.


---- SH.config ----
SH.config je velmi jednoduchý konfigurační soubor, ve kterém je uloženo jedinné číslo a to číslo portu, na kterém má poslouchat HTTP server.
Pro umístění souboru jsem zvolil "~/.config/SH/SH.config". Do složky .config se umisťují i další konfigurační soubory. Složka je přístupná a zapisovatelná pro uživatele, a pro každého uživatele je jedinečná, takže každý může mít své nastavení. Dané nastavení by mělo být přístupné pouze konnrétnímu uživateli: `chmod 700 ~/.config/SH ; chmod 700 ~/.config/SH/SH.config` (vlastníkem je uživatel)
Vlastní konfigurační soubor jsem zanořil do SH adresáře, aby se usnadnilo případné rozšiřování aplikace. (A pro lepší zpětnou kompatibilitu.)

---- NC ----
Změnu systémového času zařizuje program NC se zdrojovým souborem NC.c. NC přijímá nový čas v argumentu jako unix timestamp.
NC bude mít zvýšená oprávnění díky SUID bitu. (`chmod 4755 /usr/bin/NC`) Oprávnění se poté vzdá a přidá si pouze oprávnění CAP_SYS_TIME pro povolení operace settimeofday(). Pro nastavení capabilites jsem využil připraveného soubouru "capabilites.c", který includuji.
Binárku NC jsem uložil do složky /usr/bin/, jelikož se jedná o místo, kde jsou uloženy ostatní binárky. Běžný uživatel nemá právo zápisu do této složky. Navíc se tu nacházejí i jiné SUID soubory, jako například 'su' nebo 'sudo'.

V NC jsem se rozhodl používat při chybě exit(), pro okamžité ukončení. Při chybě NC vypisuje chybovou hlášku na chbybový výstup.
NC kontroluje počet argumentů, zda se jedná o číslo v mezích, zda se povedlo změnit čas a zda se povedlo upravit capabilites.

--- DEP ---
Linux používá DEP by default. Tudíž není nutné je explicitně zapínat.
