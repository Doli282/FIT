## Suricata

Při zpracování úlohy jsem postupoval v součinosti s https://unit42.paloaltonetworks.com/wireshark-tutorial-emotet-infection/

Ve článku a v přiložených .pcap souborech jsem identifikoval následující indikátory.

# Indication of Compromise malware Emotet

1) Domény, které jsou spjaty s šířením malware.
Hosts:
- hangarlastik.com
- padreescapes.com
- sarture.com
- seo.udaipurkart.com
- obob.tv
- miprimercamino.com
- fathekarim.com
- saketpranamam.mysquare.in
- samaritantec.com

2) Hashe infikovaných souborů.
sha256 checksum:
- 8e37a82ff94c03a5be3f9dd76b9dfc335a0f70efc0d8fd3dca9ca34dd287de1b
- 963b00584d8d63ea84585f7457e6ddcac9eda54428a432f388a1ffee21137316
- c7f429dde8986a1b2fc51a9b3f4a78a92311677a01790682120ab603fd3c2fcb

3) Potenciálně nebezpečná je .dll typ souborů.
- .dll file type

4) Komunikace s infikovanými počítači probíhala z IP adres:
- 5.2.136.90
- 167.71.4.0
- 46.101.230.194
- 71.80.66.107

5) Podezřelé bylo velké mnžoství zachycených retransmissions.

6) Komunikace s Command & Control centrem probíhalo pomocí HTTP POST requestů.

7) Spam mailbot se vyznačoval velkým SMTP přenosem na portech 25, 465, 587.

8) URI adresy infikovaných stránek byly zřejmě náhodně generované a tudíž značně "nelidské".

9) Emotet je doprovázen dalším malwarem: Trickbot, Quakbot, ...

IOC Trickbot
- HTTPS komunikace přes porty 447 a 449 bez domény a hostname
- HTTP POST request končí s /81/, /83/ nebo /90
- HTTP POST request začíná /morXX, kde XX jsou číslice
- HTTP GET přistupuje k URL končícím na .png

IOC Qakbot
- HTTPS komunikace
- komunikace přes port 65400
- zvláštní hodnoty v polích TLS certifikátu (vydavatel certifikátu nebývá z USA)
- používal adresy cdn.speedof[.]me. a a.strandsglobal[.]com.

# Suricata Rules

Pravidla jsem dle zadání rozdělil do skupin dle toho, na jakou část paketu se zaměřují.

2: HTTP Host header
= Pravidla detekující komunikaci na konkrétní stránky známé pro distribuci Emotet malware.
U těchto pravidel kontroluji odchozí komunikaci na port 80 (HTTP server) pro specifické hosty.

alert tcp ANY any -> ANY 80 (msg:"HTTP Host detected"; http.host; content:"padreescapes.com"; classtype:trojan-activity; gid:2; sid: 201; rev:1;)
alert tcp ANY any -> ANY 80 (msg:"HTTP Host detected"; http.host; content:"sarture.com"; classtype:trojan-activity; gid:2; sid: 202; rev:1;)
alert tcp ANY any -> ANY 80 (msg:"HTTP Host detected"; http.host; content:"seo.udaipurkart.com"; classtype:trojan-activity; gid:2; sid: 203; rev:1;)
alert tcp ANY any -> ANY 80 (msg:"HTTP Host detected"; http.host; content:"obob.tv"; classtype:trojan-activity; gid:2; sid: 204; rev:1;)
alert tcp ANY any -> ANY 80 (msg:"HTTP Host detected"; http.host; content:"miprimercamino.com"; classtype:trojan-activity; gid:2; sid: 205; rev:1;)
alert tcp ANY any -> ANY 80 (msg:"HTTP Host detected"; http.host; content:"fathekarim.com"; classtype:trojan-activity; gid:2; sid: 206; rev:1;)
alert tcp ANY any -> ANY 80 (msg:"HTTP Host detected"; http.host; content:"saketpranamam.mysquare.in"; classtype:trojan-activity; gid:2; sid: 207; rev:1;)
alert tcp ANY any -> ANY 80 (msg:"HTTP Host detected"; http.host; content:"samaritantec.com"; classtype:trojan-activity; gid:2; sid: 208; rev:1;)

3: HTTP URI
= Detekce na základě URI.
URI adresy stránek spojených s Emotetem se zdají býti náhodně generovanými, a tudíž nepovažuji za efektivní na ně vypisovat specifická pravidla.
Následující pravidlo kontroluje začátek URI na specifický řetězec pro Trickbot.

alert tcp ANY any -> ANY 80 (msg:"URI of probable Trickbot detected"; http.uri; content:"/mor"; startswith; classtype:misc-attack; gid:3; sid:301; rev:1;)

4: Filename
= Detekce na základě souboru.
Rozhodl jsem se detekovat file extensions, konkrétně datový typ .dll, protože se jedná o jeden z prostředů jak Emotet šířit. Emotet se šíří také soubory .docx a podobnými, nicméně ty považuji za velmi časté a tudíž by generoval extrémní množství false-positive.
DLL je možné navíc kontrolovat nejen na základě file extension ale i přímo dle signatury (DOS hlavičky) na začátku souboru.

alert tcp ANY any -> ANY any (msg:"DLL extension detected"; fileext:"dll"; classtype:trojan-activity; gid:4; sid:401; rev:1;)

5: File SHA256 hash
= Detekce hashů souborů.
Konkrétná známé hashe malwaru je možné zachytit už při toku v síti a pakety ihned zahodit. V blacklistu jsou uvedeny hashe zachyvených souborů, které obsahovaly malware.
Samozřejmě by bylo bezpečenéjší použíat whitelisting. Zde ovšem záleží na business logic dané organizace, zda je takový přístup možný vzhledem k objemu poslaných dokumentů.

drop tcp ANY any -> ANY any (msg:"FILE SHA256 detected"; filesha256:sha256-blacklist; classtype:trojan-activity; gid:5; sid:501; rev:1;)

6: Konkrétní IP adresy nebo porty
= filtrování dle IP adresy a portu

Pakety mířící k/od adres, které jsou spojeny s šířením malware jsou zahozeny.

drop tcp ANY any <> 5.2.136.90 any (msg:"TCP Port detected"; classtype:command-and-control; target:src_ip; gid:6; sid:601; rev:1;)
drop tcp ANY any <> 167.71.4.0 any (msg:"TCP Port detected"; classtype:command-and-control; target:src_ip; gid:6; sid:602; rev:1;)
drop tcp ANY any <> 46.101.230.107 any (msg:"TCP Port detected"; classtype:command-and-control; target:src_ip; gid:6; sid:603; rev:1;)
drop tcp ANY any <> 71.80.66.107 any (msg:"TCP Port detected"; classtype:command-and-control; target:src_ip; gid:6; sid:604; rev:1;)

Při častějším posíláním paketů přes na porty určené pro SMTP server vzniká podezření, že komunikace pochází od mailbota. Při silnější komunikaci je proto vygenerováno upozornění. 

alert tcp ANY any -> ANY [25,465,587] (msg:"SMTP sus activity detected"; flow:established; threshold: type both, track by_src, count 10, seconds 120; classtype:misc-activity; gid:6; sid:605; rev:1;)

7: Políčka TLS certifikátu
= Detekce možných nevalidních a falšovaných certifikátů.
Nejbezpečnější, ale také nepohodlné by bylo vybrat několik certifikačních autorit, které by bylo umístěny na whitelist. Ostatní certifikáty by byly nedůvěryhodné.

V rámci této úlohy jsem připravil pravidlo na detekci certifikačních autorit, které nepocházejí z USA, a pro uživatele z USA by se tak mohlo jednat o nedůvěryhodné certifikáty.

alert tcp ANY any -> ANY any (msg:"TLS with potentially forged certificate detected"; tls.cert_issuer; pcre:"/(?!US)/"; threshold: type limit, track by_src, seconds 120, count 1; classtype:non-standard-protocol; gid:8; sid:801; rev:1;)

8: Regex matchování pomocí prce
Využito při matchování vydavatele certifikátu výše.

# Run
Při spuštění na připravené .pcap soubory se vygeneruje několik alertů.

Použtý příkaz byl:
suricata -r ~/wireshark-tutorial-Emotet-traffic/ -c ~/wireshark-tutorial-Emotet-traffic/suricata.yaml -l ~/wireshark-tutorial-Emotet-traffic/logs/

v ~/wireshark-tutorial-Emotet-traffic/ se nacházejí připravené .pcap soubory
přičemž suricata.rules je připraven v /var/lib/suricata/rules/

v /logs se následovně vygeneruje
- suricata.log
- eve.json
- eve-alerts.json (pomocí jq -c 'select(.event_type == "alert")' eve.json > eve-alerts.json)
- fast.log
- stats.log