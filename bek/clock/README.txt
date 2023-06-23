Aplikaci jsem napsal pro unixový operační systém.
Aplikace se skládá z python skriptu 'SH.py', konfiguračního souboru 'SH.config' a binárního souboru 'NC'.

---- SH.py ----
Python skript (hlavní aplikaci) jsem umístil do /usr/bin/, jelikož se jedná o místo, kde jsou i jiné spustitelné soubory. Běžný uživatel sem nemá právo zápisu a já nechci, aby bylo možno skript změnit. Nicméně, skript ponechávám čitelný. (`chmod 0755 /usr/bin/SH.py`)
SH.py běží vícevláknově:
Vedlejší vlákno spustí HTTP server, který odbavuje požadavky vzdálených uživatelů a vrací jim aktuální čas v jimi zadefinovaném formátu. Port, na kterém server poběží si aplikace přečte v konfiguračním souboru.
Pro získávání požadavů od vzdálených uživatelů přijímá server GET reuqesty s hodnotou argumentu. Vybraný formátovací řetězec je vložen právě do argumentu URL. Na stránce se poté zobrazí výsledek.
Pokud se server nepovede z jakýchkoli důvodů spustit, spustí se pouze lokální část aplikace.

Hlavní vlákno je pro lokálního uživatele. Lokální uživatel může taktéž zadat formát, ve kterém se má vypsat aktuální čas. Navíc může přejít do nastavovacího módu příkazem "set". Poté v definovaném formátu zadá nový čas. Tím se spustí aplikace NC, která změnu provede.

Pro validace vstupů a výpisu času dle vstupního formátovacího řetězce od uživatele využívám knihovní funkce pythonu z knihovny datetime - strptime a strftime. Ty validaci provádí samy.
Buffer overflow řeší python dynamickou alokací až do sys.maxsize, pak vyhodí výjimku.

Skript se zbavuje všech capabilities, žádná nejsou potřeba.

---- SH.config ----
SH.config je velmi jednoduchý konfigurační soubor, ve kterém je uloženo jedinné číslo a to číslo portu, na kterém má poslouchat HTTP server.
Pro umístění souboru jsem zvolil "~/.config/SH/SH.config". Do složky .config se umisťují i další konfigurační soubory. Složka je přístupná a zapisovatelná pro daného uživatele, a pro každého uživatele je jedinečná, takže každý může mít své nastavení. Dané nastavení by mělo být přístupné pouze konnrétnímu uživateli: `chmod 700 ~/.config/SH ; chmod 700 ~/.config/SH/SH.config` (vlastníkem je uživatel)
Vlastní konfigurační soubor jsem zanořil do SH adresáře, aby se usnadnilo případné rozšiřování aplikace o další konfigurační soubory. (A pro lepší zpětnou kompatibilitu.)

---- NC ----
Změnu systémového času zařizuje program NC se zdrojovým souborem NC.c. NC přijímá nový čas v argumentu jako unix timestamp.
NC bude mít zvýšená oprávnění díky SUID bitu. (`chmod 4755 /usr/bin/NC` + vlastník je root) Oprávnění se poté vzdá a přidá si pouze oprávnění CAP_SYS_TIME pro povolení operace settimeofday(). Pro nastavení capabilites jsem využil připraveného soubouru "capabilites.c", který includuji.
Binárku NC jsem uložil do složky /usr/bin/, jelikož se jedná o místo, kde jsou uloženy i ostatní binárky. Běžný uživatel nemá právo zápisu do této složky, takže binárku nepodvrhne. Vzhledem k tomu, že jiné binárnky jsou čitelné, ponechávám i u NC právo číst. Navíc se tu nacházejí i jiné SUID soubory, jako například 'su' nebo 'sudo'.

V NC jsem se rozhodl používat při chybě exit(), pro okamžité ukončení. Při chybě NC vypisuje chybovou hlášku na chybový výstup.
NC kontroluje počet argumentů, zda se jedná o číslo v mezích, zda se povedlo změnit čas a zda se povedlo upravit capabilites.

--- DEP ---
Linux používá DEP by default. Tudíž není nutné je explicitně zapínat.

V Linuxu je tato featura nazývaná též No Execute, Execute Disable, případně Enhanced Virus Protection.

Patch linuxového jádra z roku 2004:
Pokud CPU čip podporuje NX, jádro NX bit nahodí.
https://lkml.iu.edu/hypermail/linux/kernel/0406.0/0497.html
"If the NX feature is supported by the CPU then the patched kernel turns
on NX and it will enforce userspace executability constraints such as a
no-exec stack and no-exec mmap and data areas. This means less chance
for stack overflows and buffer-overflows to cause exploits."

Článek od Redhat z roku 2020:
https://access.redhat.com/solutions/2936741
"If the cpu has this feature (NX/XD) it is enabled by default , Unless it is over-ridden by noexec=off parameter."

Záleží také na konkrétní distribuci Linuxu. Například Oracle Linux vůbec neumožňuje DEP vypnout.
https://docs.oracle.com/en/operating-systems/oracle-linux/6/security/ol_dep_sec.html
"You cannot disable the DEP feature."

Vyšší verze Ubuntu mají NX zapnutý nehledě na nasatavení BIOSu
https://wiki.ubuntu.com/Security/Features#nx
"Starting in Ubuntu 11.04, BIOS NX settings are ignored by the kernel."

Nastavení linoxvé cmd má defaultně noexec zapnutý:
https://www.kernel.org/doc/Documentation/admin-guide/kernel-parameters.txt
"noexec32=on: enable non-executable mappings (default)
				read doesn't imply executable mappings"