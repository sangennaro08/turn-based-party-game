/*

che cosa deve mandare e ricevere il server py grazie a c++?

1)a ogni fine round aggiornare lo status dei player e bot.
2)implementare la funzione di rollDices e inviare via json i risultati in base al dado usato
3)ogni tot round finiti(tot deciso dall'utente ci sarà un minigame di blackjack/poker dove si scomettere argento o oro)
4)controllo dell'inventario, di quando si usa un oggetto, chiamare il server per modificare lo status
5)tiro dei dadi: in base al dado scelto deve...
OPTIONAL: se è un bot lui userà uin algoritmo per capire la casella a lui più favorevole, se è un player allora non serve
e vai direttamente a tirare il daso normalmente

a:in base al dado scelto il front end userà un dado specifico e fà l'animazione del tiro
b:con il tiro mando una richiesta al server di usare rolldice in base al dado usato
c:prendere il dado dall inventario e fare la cosa del punto 4
d:modificare lo status del player/bot in base al tiro fatto

6)creare o eliminare oggetti se usati
7)
*/