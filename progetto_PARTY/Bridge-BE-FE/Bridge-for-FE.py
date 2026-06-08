from typing import Any, Dict
import subprocess
import json
import os
import sys

from fastapi import FastAPI
from pydantic import BaseModel

app = FastAPI()

# Percorso dell'eseguibile C++
# Adjust this based on where you compile the C++ core
BACKEND_DIR = os.path.join(os.path.dirname(__file__), "..", "backend")
CPP_EXECUTABLE = os.path.join(BACKEND_DIR, "core.exe")

# Processo globale del core C++
cpp_process = None


def start_cpp_core():
    """Avvia il processo C++ all'inizio"""
    global cpp_process
    try:
        cpp_process = subprocess.Popen(
            [CPP_EXECUTABLE],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            bufsize=1  # Line buffering
        )
        print(f"C++ core started: {CPP_EXECUTABLE}", file=sys.stderr)
    except Exception as e:
        print(f"Failed to start C++ core: {e}", file=sys.stderr)
        cpp_process = None


def call_cpp_core(action: str, player_id: int, payload: Dict[str, Any] = None) -> Dict[str, Any]:
    """Invia un comando al core C++ e riceve la risposta"""
    global cpp_process
    
    if cpp_process is None:
        return {"status": "error", "message": "C++ core not running"}
    
    try:
        # Crea il messaggio JSON
        message = {
            "action": action,
            "player_id": player_id,
            "payload": payload or {}
        }
        
        # Invia a C++ via stdin
        cpp_process.stdin.write(json.dumps(message) + "\n")
        cpp_process.stdin.flush()
        
        # Leggi la risposta da stdout
        response_line = cpp_process.stdout.readline()
        if not response_line:
            return {"status": "error", "message": "No response from C++ core"}
        
        response = json.loads(response_line)
        return response
    except Exception as e:
        return {"status": "error", "message": str(e)}


class ActionRequest(BaseModel):
    player_id: int
    action: str
    payload: Dict[str, Any] = {}


@app.on_event("startup")
def startup():
    """Avvia il core C++ all'avvio di FastAPI"""
    start_cpp_core()


@app.on_event("shutdown")
def shutdown():
    """Chiudi il core C++ alla chiusura"""
    global cpp_process
    if cpp_process:
        cpp_process.terminate()
        cpp_process.wait()


@app.get("/api/ping")
def ping() -> Dict[str, str]:
    return {"status": "ok", "message": "bridge active"}


@app.get("/api/game-state")
def get_game_state() -> Dict[str, Any]:
    # Qui il bridge restituisce lo stato di gioco.
    # In una versione reale, questa funzione può richiamare il motore C++
    # tramite subprocess, socket locale, o API locali.
    return {
        "turn": 1,
        "players": [
            {"id": 1, "name": "Alice", "coins": 10, "position": 3},
            {"id": 2, "name": "Bob", "coins": 8, "position": 5},
        ],
        "board": []
    }


@app.post("/api/action")
def post_action(request: ActionRequest) -> Dict[str, Any]:
    # Questo endpoint riceve le azioni del frontend.
    # Il bridge invia la richiesta al core C++ e restituisce la risposta.
    result = call_cpp_core(
        request.action,
        request.player_id,
        request.payload
    )
    return result
 