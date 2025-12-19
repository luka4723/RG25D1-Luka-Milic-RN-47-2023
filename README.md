```text
Funkcionalnosti:  
  Moguće je kretanje automobila na WASD. Auto ne može otići van ekrana i ne može da se skloni sa ulice.
  Iz auspuha idu particles koji su blendovanjem providni.
  Nasumično se stvaraju objekti koje auto može da pokupi:  
      Pečurka: Meša ceo ekran sa kvadratom obojenim bilinearnom interpolacijom.
               Jačina i boje efekta se nasumično biraju. Efekat se postepeno smanjuje sve dok ne nestane.
               Dok traje pečurka čestice iz auspuha menjaju boju.  
      Napitak: Invertuje kretanje automobila na 5 sekundi.  
      Flaša:   Pravi se direkcioni blur "u spiralu" - vektor se krece u krug i smanjuje se sve dok ne postane 0
               Za ovo se koristi framebuffer.
      Blato:   Kada auto prođe kroz blato na ekranu se pomoću stencila iscrtavaju random krugovi. Ako auto prođe
               kroz više blata u kratkom vremenu, mrlje se akumuliraju. Posle nekoliko sekundi se čiste.
  Na P se stavlja pauza. Dok traje pauza boje ekrana su invertovane i čuvaju se stanje direkcionog vektora i
      intenzitet pečurke.
  Na R se rotira ekran.
  na ESC se izlazi iz igre
  Ulica se generiše biranjem nasumičnih tile-ova
```
    
