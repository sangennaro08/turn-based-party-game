import json
import os
import sys
import uvicorn
from typing import Any, Dict, List, Optional
from fastapi import FastAPI, HTTPException, status
from fastapi.middleware.cors import CORSMiddleware
from fastapi_socketio import SocketManager
from pydantic import BaseModel

import sys
sys.path.append("build")

#squirrel normale, legge lo stesso però vscode non riconosce bene i file binari
import Comunication

try:
    # 1. Inizializza un giocatore e un bot
    giocatore = Comunication.Player("Mario", "ID_PLAYER_1")
    bot = Comunication.Bot("LuigiBot", "ID_BOT_1", "hard")
    
    print(f"Giocatore creato: {giocatore.name}")
    print(f"Bot creato: {bot.name} (Difficoltà: {bot.difficulty})")
    
    # 2. Controlla l'inventario iniziale
    print(f"Monete d'argento iniziali di Mario: {giocatore.Inv.silverCoins}")
    
    print(Comunication.ItemPrices.budget.value)

    # 3. Prova ad aggiungere un oggetto (es. il DoubleDice che creava problemi)
    dado_doppio = Comunication.DoubleDice()
    giocatore.Inv.AddItem(dado_doppio)
    print("Dado doppio aggiunto all'inventario con successo!")

except Exception as e:
    print(f"Errore durante il test del modulo C++: {e}")


"""
-
questo è il server python, zona centrale del progetto, è il punto di contatto tra il frontend e il backend in C++. 
Il server riceve le richieste dal frontend, le elabora e chiama le funzioni del backend in C++ tramite ctypes.
Inoltre, gestisce la comunicazione in tempo reale con il frontend usando Socket.IO.

liberie in utilizzo:

ctypes: permette di interagire con librerie scritte in C NB: il formato che chiama varia a seconda dell'OS

uvicorn: server molto utilizzato con fastAPI che manda e riceve richieste HTTP inoltrandole a fastAPI
esempio...
Uvicorn

Ascolta su una porta (es. 0.0.0.0:8000).
Riceve le connessioni da browser, app mobili, frontend, ecc.
Traduce le richieste HTTP/WebSocket nel formato ASGI.
Passa la richiesta a FastAPI.
Riceve la risposta da FastAPI e la invia al client.

typing: libreria che permette di definire i tipi alle variabili con Any(ogni  tipo) e altri

FastAPI: framework che permette di ricevere info, e dare la risposta voluta
esempio...

FastAPI

Riceve la richiesta da Uvicorn.
Capisce quale endpoint deve eseguirla (/users, /login, ecc.).
Valida i dati ricevuti.
Esegue il tuo codice Python.
Genera la risposta (JSON, HTML, file, ecc.).
Restituisce la risposta a Uvicorn.

"""

# Inizializzazione di FastAPI
#app = FastAPI(title="Game Server Bridge", version="2.0.0")

# Configurazione CORS
#app.add_middleware(
#    CORSMiddleware,
#    allow_origins=["*"],
#    allow_credentials=True,
#    allow_methods=["*"],
#    allow_headers=["*"],
#)

# Initializing the Socket.io
#sio_manager = SocketManager(app=app, cors_allowed_origins="*")
#sio = sio_manager._sio