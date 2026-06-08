#!/usr/bin/env python3
"""
Game Server Bridge - Python router tra C++ backend e frontend web
Solo gateway - niente logica di gioco qui!
Versione: FastAPI + FastAPI-SocketIO (Generata nel 2026)
"""

import ctypes
import json
import os
import sys
from typing import Any, Dict, List, Optional
from fastapi import FastAPI, HTTPException, status
from fastapi.middleware.cors import CORSMiddleware
from fastapi_socketio import SocketManager
from pydantic import BaseModel

# Inizializzazione di FastAPI
app = FastAPI(title="Game Server Bridge", version="2.0.0")

# Configurazione CORS (Sostituisce CORS(app) di Flask)
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# Inizializzazione di Socket.io (Sostituisce SocketIO(app))
sio_manager = SocketManager(app=app, cors_allowed_origins="*")
sio = sio_manager._sio

# ==================== CARICAMENTO C++ (CTYPES) ====================
cpp_lib = None
try:
    if os.name == 'nt':
        cpp_lib = ctypes.CDLL('./build/Release/gamecpp.dll')
    else:
        cpp_lib = ctypes.CDLL('./build/libgamecpp.so')
    print(" [BRIDGE] C++ loaded")
except OSError as e:
    print(f" [BRIDGE] C++ not found: {e}")

# Configurazione firme funzioni C++
if cpp_lib:
    cpp_lib.init_game.argtypes = [ctypes.c_char_p]
    cpp_lib.init_game.restype = ctypes.c_char_p
    
    cpp_lib.get_entity_state.argtypes = [ctypes.c_int]
    cpp_lib.get_entity_state.restype = ctypes.c_char_p
    
    cpp_lib.activate_dice.argtypes = [ctypes.c_int, ctypes.c_char_p]
    cpp_lib.activate_dice.restype = ctypes.c_char_p
    
    cpp_lib.roll_dice.argtypes = [ctypes.c_int, ctypes.c_char_p]
    cpp_lib.roll_dice.restype = ctypes.c_char_p
    
    cpp_lib.activate_item.argtypes = [ctypes.c_int, ctypes.c_char_p]
    cpp_lib.activate_item.restype = ctypes.c_char_p
    
    cpp_lib.use_item.argtypes = [ctypes.c_int, ctypes.c_int]
    cpp_lib.use_item.restype = ctypes.c_char_p
    
    cpp_lib.get_inventory.argtypes = [ctypes.c_int]
    cpp_lib.get_inventory.restype = ctypes.c_char_p
    
    cpp_lib.add_coins.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.c_int]
    cpp_lib.add_coins.restype = ctypes.c_char_p
    
    cpp_lib.remove_coins.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.c_int]
    cpp_lib.remove_coins.restype = ctypes.c_char_p
    
    cpp_lib.get_game_state.argtypes = []
    cpp_lib.get_game_state.restype = ctypes.c_char_p
    
    cpp_lib.test_bridge.argtypes = []
    cpp_lib.test_bridge.restype = ctypes.c_char_p


def call_cpp(func_name: str, *args: Any) -> Dict[str, Any]:
    """Chiama funzione C++ e ritorna JSON"""
    if not cpp_lib:
        return {"status": "error", "message": "C++ not available"}
    try:
        func = getattr(cpp_lib, func_name)
        c_args = [arg.encode('utf-8') if isinstance(arg, str) else arg for arg in args]
        result_ptr = func(*c_args)
        if result_ptr:
            return json.loads(ctypes.string_at(result_ptr).decode('utf-8'))
        return {"status": "error", "message": "Empty response"}
    except Exception as e:
        print(f" [ERROR] {func_name}: {e}", file=sys.stderr)
        return {"status": "error", "message": str(e)}


# ==================== PYDANTIC MODELS (VALIDAZIONE DATI) ====================
class InitGameRequest(BaseModel):
    entities: List[Any] = []

class DiceRequest(BaseModel):
    dice_type: str = "standard"

class ItemRequest(BaseModel):
    item_type: str

class UseItemRequest(BaseModel):
    target_entity_id: int = 0

class CoinsRequest(BaseModel):
    silver: int = 0
    gold: int = 0


# ==================== REST ENDPOINTS ====================

@app.get('/api/health')
def health():
    return {'status': 'online', 'cpp': cpp_lib is not None}


@app.post('/api/game/init', status_code=status.HTTP_201_CREATED)
def init_game(request_data: InitGameRequest):
    result = call_cpp('init_game', json.dumps(request_data.entities))
    if result.get('status') in ['initialized', 'success']:
        return result
    raise HTTPException(status_code=status.HTTP_400_BAD_REQUEST, detail=result)


@app.get('/api/entity/{entity_id}/state')
def get_entity_state(entity_id: int):
    result = call_cpp('get_entity_state', entity_id)
    return result


@app.post('/api/entity/{entity_id}/activate-dice')
async def activate_dice(entity_id: int, request_data: DiceRequest):
    result = call_cpp('activate_dice', entity_id, request_data.dice_type)
    if result.get('status') == 'success':
        await sio.emit('dice_activated', result)
    return result


@app.post('/api/entity/{entity_id}/roll-dice')
async def roll_dice(entity_id: int, request_data: Optional[DiceRequest] = None):
    # Gestisce anche il body vuoto ({}) assegnando i valori di default del modello
    dice_type = request_data.dice_type if request_data else "standard"
    result = call_cpp('roll_dice', entity_id, dice_type)
    if result.get('status') == 'success':
        await sio.emit('dice_rolled', result)
    return result


@app.post('/api/entity/{entity_id}/activate-item')
async def activate_item(entity_id: int, request_data: ItemRequest):
    result = call_cpp('activate_item', entity_id, request_data.item_type)
    if result.get('status') == 'success':
        await sio.emit('item_activated', result)
    return result


@app.post('/api/entity/{entity_id}/use-item')
async def use_item(entity_id: int, request_data: Optional[UseItemRequest] = None):
    target_id = request_data.target_entity_id if request_data else 0
    result = call_cpp('use_item', entity_id, target_id)
    if result.get('status') == 'success':
        await sio.emit('item_used', result)
        print(f" [BRIDGE] Item used by entity {entity_id}")
    return result


@app.get('/api/entity/{entity_id}/inventory')
def get_inventory(entity_id: int):
    result = call_cpp('get_inventory', entity_id)
    return result


@app.post('/api/entity/{entity_id}/add-coins')
async def add_coins(entity_id: int, request_data: CoinsRequest):
    result = call_cpp('add_coins', entity_id, request_data.silver, request_data.gold)
    if result.get('status') == 'success':
        await sio.emit('coins_updated', result)
    return result


@app.post('/api/entity/{entity_id}/remove-coins')
async def remove_coins(entity_id: int, request_data: CoinsRequest):
    result = call_cpp('remove_coins', entity_id, request_data.silver, request_data.gold)
    if result.get('status') == 'success':
        await sio.emit('coins_updated', result)
    return result


@app.get('/api/game/state')
def get_game_state():
    result = call_cpp('get_game_state')
    return result


@app.get('/api/test')
def test():
    result = call_cpp('test_bridge')
    return result


# ==================== SOCKET.IO EVENTS ====================

@sio.on('connect')
async def on_connect(sid, environ):
    print(f"[SOCKET] Connected: {sid}")
    await sio.emit('connected', {'message': 'Connected to game server'}, room=sid)


@sio.on('join_game')
async def on_join_game(sid, data):
    game_id = data.get('game_id')
    if game_id:
        sio.enter_room(sid, game_id)
        await sio.emit('player_joined', data, room=game_id)


if __name__ == '__main__':
    import uvicorn
    print("[BRIDGE] Starting on http://0.0.0.0:5000")
    # Avvia uvicorn (il server ASGI standard per FastAPI) al posto di socketio.run
    uvicorn.run(app, host='0.0.0.0', port=5000, log_level="info")