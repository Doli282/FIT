# Úkol 8: REST API

Vaším úkolem je vytvořit jednoduché REST API pro správu knih. Pomocí API lze prohlížet existující knihy, vytvářet nové a upravovat a mazat existující.

Jako persistentní úložitě budeme používat Sqlite a do něj budeme ukládat informace o knihách. K ukládání bude sloužit tabulka `books` a bude mít následující sloupce:

- `id`
- `name`
- `author`
- `publisher`
- `isbn`
- `pages`

Testovací prostředí **závisí** na konkrétních názvech sloupců; prosíme o jejich dodržení.

Prohlížení existujících záznamů může dělat kdokoliv. Operace, které záznamy upravují mohou dělat pouze autorizovaní uživatelé,
kdy ověření probíhá pomocí HTTP Basic Auth. Pro účely tohoto úkolu stačí "zahardcodovat" uživatele `admin` s heslem `pas$word`.
Pro získávání hesla a loginu použijte `$request->getHeader('Authorization')` např. v nějakém middlewaru.

Máte připravenou kostru aplikace v `public/index.php` a `src/Rest/RestApp`. Implementujte metodu `configure()` ve třidě `RestApp`, zbytek třídy neměnte.

Máte také připravenou dokumentaci endpointů v openapi specifikaci ve složce [`docs`](docs/openapi.yaml). Tu si můžete zobrazit v PHP stormu a nebo v libovolném editoru openAPI, třeba swagger.

Není potřeba implementovat vše v do jedné metody, máte nastavený namespace `Books` do složky `src`,
vytvořte si další třídy, které budete potřebovat, aby byl kód přehledný.


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
7. Pro Sql Lite stačí zvolit soubor s touto db po prvním volání. Soubor se objeví v kořenu projektu.
8. Vaše appka by měla být dostupna na adrese http://localhost:8000/
9. Přípravené testy můžete pustit pomocí příkazu: `composer test`

Pozor pokud používate docker, všechny tyto přikazy musíte volat uvnitř kontejneru a nebo nastavit si remote interpretor v php stormu.


---

## Seznam uložených knih

**Request**

```
> GET /books

```

**Success Response**

Vrátí seznam uložených knih. V případě, že žádné knihy uložené nejsou, vrátí prázdný seznam. Seznam knih obsahuje pouze `id`, `name` a `author`.

```
< 200 OK

[{
    "id": 1,
    "name": "The Little Prince",
    "author": "Antoine de Saint-Exupéry"  
}, {
    ...
}]
```

---

## Detail knihy

**Request**

```
> GET /books/:id

```

**Success Response**

Vrátí detail knihy, který obsahuje všechna pole.

```
< 200 OK

{
    "id": 1,
    "name": "The Little Prince",
    "author": "Antoine de Saint-Exupéry",
    "publisher": "Mariner Books",
    "isbn": "978-0156012195",
    "pages": 96
}
```

**Error Response**

V případě neexistujícího `id` vrátí HTTP chybu 404.

```
< 404 Not Found

```

V případě špatně zadaného `id` vrátí HTTP chybu 400 (např. není to číslo).

```
< 400 Bad Request

```

---

## Vytvoření nové knihy 🔐

**Request**

Novou knihu může vytvořit pouze autorizovaný uživatel. To je ověřeno pomocí HTTP Basic Auth. Je potřeba poslat všechny informace o knize.

```
> POST /books

Authorization: Basic <token>
Content-Type: application/json


{
    "name": "The Little Prince",
    "author": "Antoine de Saint-Exupéry",
    "publisher": "Mariner Books",
    "isbn": "978-0156012195",
    "pages": 96
}
```

**Success Response**

Server automaticky vygeneruje `id` nové knihy a vrátí hlavičku `Location`, která obsahuje URL nové knihy.

```
< 201 Created

Location: /books/:id
```

**Unauthorized Error Response**

Pokud uživatel nepošle správný token nebo ho nepošle vůbec, vrátí server HTTP chybu 401.

```
< 401 Unauthorized

```

**Bad Request Error Response**

Pokud request neobsahuje všechny informace o knize, vrátí server HTTP chybu 400. Pokud chcete, můžete v odpovědi vrátit i informace o chybějících datech (ve formátu JSON).

```
< 400 Bad Request

```

---

## Aktualizace existující knihy 🔐

**Request**

Aktualizovat existující knihu může opět pouze autorizovaný uživatel. Pošle všechny informace znovu a existující záznam je jimi zcela nahrazen, `id` zůstává stejné.

```
> PUT /books/:id

Authorization: Basic <token>
Content-Type: application/json


{
    "name": "The Little Prince",
    "author": "Antoine de Saint-Exupéry",
    "publisher": "Mariner Books",
    "isbn": "978-0156012195",
    "pages": 96
}
```

**Success Response**

V případě úspěchu server nic nevrací a odpoví HTTP statusem 204.

```
< 204 No Content

```

**Unauthorized Error Response**

Pokud uživatel nepošle správný token nebo ho nepošle vůbec, vrátí server HTTP chybu 401.

```
< 401 Unauthorized

```

**Not Found Error Response**

Pokud je uživatel správně autorizovaný, ale snaží se aktualizovat neexistující záznam, vrátí server HTTP chybu 404.

```
< 404 Not Found

```

**Bad Request Error Response**

Stejně jako v případě vytváření nové knihy, je i zde potřeba ověřit, že jsou odeslaná všechna data.

```
< 400 Bad Request

```

---

## Smazání knihy 🔐

**Request**

Knihu může smazat pouze autorizovaný uživatel.

```
> DELETE /books/:id

Authorization: Basic <token>

```

**Success Response**

V případě úspěchu server nevrací nic, odpoví HTTP statusem 204.

```
< 204 No Content

```

**Unauthorized Error Response**

Pokud uživatel nepošle správný token nebo ho nepošle vůbec, vrátí server HTTP chybu 401.

```
< 401 Unauthorized

```

**Not Found Error Response**

Pokud je uživatel správně autorizovaný, ale snaží se smazat neexistující záznam, vrátí server HTTP chybu 404.

```
< 404 Not Found

```
