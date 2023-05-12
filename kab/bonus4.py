
#!/usr/bin/python3
import decimal
from numpy import cbrt
decimal.getcontext().prec = 1000

# ----------------------------------------------------------------------------------------------------------------------------------------------------------
# ZADANI:

# Pomocí RSA jsme zašifrovali zprávu pro 3 příjemce. Níže jsou uvedené jejich veřejné klíče (e,n) a šifrové texty c. Zjistěte zprávu p.

n1 = 12187807089357197827615596944196239086089873906697497704427922056628877930377064451068748274278762548877934585146060850782744967979399605496097900090082333
n2 = 7132068923992193768679628518376931381604894715339307874966353868159933509426415225822624563196167025753578726119585937526686628009797842836425564496351013
n3 = 7779588303740118346050855450506137442690268404234916544889839303341856550723269598807803147406096318113211676639901315377544940189170478934766550846321647

e = [3, 3, 3]

c1 = 5202643082447192293534278658357251311844742222536812652658126156448733527979723170368898619928452011589011630957302338560219485301334995886050736407905323
c2 = 785735371551248336853643206947132459999048377530846108045086587142242191848854042039938160565050239549984745212278867425806606501872903729119864222751680
c3 = 5412157552239309425253848637249740736384877769288131364712439415837877980930005343636677004531011597951967996899200952928655705700159263026695414606371449
# ----------------------------------------------------------------------------------------------------------------------------------------------------------
# RESENI:
# (vypracoval: Dolansky Lukas)

# Håstad's broadcast attack (zdroj: https://en.wikipedia.org/wiki/Coppersmith's_attack#H.C3.A5stad.27s_broadcast_attack)
# Pouziti stejneho maleho exponentu a dany pocet sifrovych textu umoznuje vyuzit Cinskou vetu o zbytcich

# Cinska Veta o Zbytcich: 
# pro soustavu kongruenci:
# c_sum = c1 (mod n1)
# c_sum = c2 (mod n2)
# c_sum = c3 (mod n3)

# c_sum = (c1*n2*n3*((n2*n3)^-1 mod n1) + c2*n1*n3*((n1*n3)^-1 mod n2) + c3*n1*n2*((n1*n2)^-1 mod n3))  mod (n1*n2*n3)

# c_sum = Message ^ 3 (mod n1*n2*n3)


# Spocteni inverznich clenu
d12 = pow(n1*n2, -1, n3)
d13 = pow(n1*n3, -1, n2)
d23 = pow(n2*n3, -1, n1)

# Spocteni jednotlivych citatelu
a0 = c1 * n2 * n3 * d23
a1 = c2 * n1 * n3 * d13
a2 = c3 * n1 * n2 * d12

# Spocteni celeho c_sum
c_sum = (a0 + a1 + a2) % (n1 * n2 * n3)

# Vypocteni puvodni zpravy jako treti odmocnina z c_sum
p = decimal.Decimal(c_sum) ** (decimal.Decimal('1') / 3)

# Vysledek
print("Zachycena zprava je:")
print(round(p,0)) # zaokrouhleni numericke chyby - z podstaty by se melo jednat o cele cislo

# p = 4959574536758669058216395416565680108194355765432704932058050758840730326398332851086238082711883734672820047693532232710096463411229859439566801460516246