#ifndef __HELP_H__
#define __HELP_H__
const char help[] =
    "Report permette di visualizzare i risultati delle analisi \n"
    "in maniera semplice e dinamica.\n"
    "Nonostante sia consigliato eseguire questo programma assieme a un\n"
    "solo analyser, anche il caso in cui ce ne siano multipli è gestito.\n"
    "Appositi flags permettono di decidere, anche durante l'esecuzione,\n"
    "come si vuole che vengano presentati i dati.\n"
    "\n"
    "Le opzioni per cambiare la visualizzazione sono le seguenti:\n"
    "-t      Mostra i risultati dell'analisi in una tabella.\n"
    "        è la visualizzazione di default.\n"
    "-v      Stampa le statisiche di ogni singolo file analizzato\n"
    "        uno dopo l'altro. (Molto verboso)\n"
    "-c		 Mostra una visualizzazione alternativa alla tabella ma \n"
    "        comunque relativamente compatta.\n"
    "--only  Questo flag, seguito da una lista di percorsi permette\n"
    "        di vedere le statistiche relative a tutti e soli i files\n"
    "        specificati.\n"
    "        Nel caso il percorso sia una cartella, saranno visualizzati\n"
    "        tutti i file contenuti in essa"
    "\n"
    "Di default le statistiche sono raggruppate per minuscole, maiuscole,\n"
    "numeri, punteggiatura e spazi ma con il flag -e è possibile,\n"
    "in tutte le visualizzazioni tranne -t, visualizzare le statistiche\n"
    "dei singoli caratteri.\n"
    "-h		help: mostra questa schermata\n"
    "-r		reset: cancella i dati attuali di tutte le analisi\n"
    "-q		quit: chiude il programma\n";

#endif
