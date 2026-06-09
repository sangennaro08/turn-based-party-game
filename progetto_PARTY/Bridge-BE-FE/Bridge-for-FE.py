#!/usr/bin/env python3
"""
Game Server Bridge - Python router tra C++ backend e frontend web
Solo gateway - niente logica di gioco qui!
"""

from flask import Flask, request, jsonify
from flask_cors import CORS
from flask_socketio import SocketIO, emit, join_room
import ctypes
import json
import os
import sys

app = Flask(__name__)
CORS(app)
app.config['SECRET_KEY'] = 'party-game-2026'
socketio = SocketIO(app, cors_allowed_origins="*", async_mode='threading')

# Carica C++
cpp_lib = None
try:
    if os.name == 'nt':
        cpp_lib = ctypes.CDLL('./build/Release/gamecpp.dll')
    else:
        cpp_lib = ctypes.CDLL('./build/libgamecpp.so')
    print(" [BRIDGE] C++ loaded")
except OSError as e:
    print(f" [BRIDGE] C++ not found: {e}")

# Configure C++ functions
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

def call_cpp(func_name, *args):
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

# REST ENDPOINTS
@app.route('/api/health', methods=['GET'])
def health():
    return jsonify({'status': 'online', 'cpp': cpp_lib is not None})

@app.route('/api/game/init', methods=['POST'])
def init_game():
    data = request.json
    entities = data.get('entities', [])
    result = call_cpp('init_game', json.dumps(entities))
    if result.get('status') in ['initialized', 'success']:
        return jsonify(result), 201
    return jsonify(result), 400

@app.route('/api/entity/<int:entity_id>/state', methods=['GET'])
def get_entity_state(entity_id):
    result = call_cpp('get_entity_state', entity_id)
    return jsonify(result)

@app.route('/api/entity/<int:entity_id>/activate-dice', methods=['POST'])
def activate_dice(entity_id):
    data = request.json
    result = call_cpp('activate_dice', entity_id, data.get('dice_type', 'standard'))

    if result.get('status') == 'success':
        socketio.emit('dice_activated', result, broadcast=True)

    return jsonify(result)

@app.route('/api/entity/<int:entity_id>/roll-dice', methods=['POST'])
def roll_dice(entity_id):
    data = request.json or {}
    result = call_cpp('roll_dice', entity_id, data.get('dice_type', 'standard'))

    if result.get('status') == 'success':
        socketio.emit('dice_rolled', result, broadcast=True)

    return jsonify(result)

@app.route('/api/entity/<int:entity_id>/activate-item', methods=['POST'])
def activate_item(entity_id):
    data = request.json
    result = call_cpp('activate_item', entity_id, data.get('item_type'))

    if result.get('status') == 'success':
        socketio.emit('item_activated', result, broadcast=True)

    return jsonify(result)

@app.route('/api/entity/<int:entity_id>/use-item', methods=['POST'])
def use_item(entity_id):
    data = request.json or {}
    result = call_cpp('use_item', entity_id, data.get('target_entity_id', 0))

    if result.get('status') == 'success':
        socketio.emit('item_used', result, broadcast=True)
        print(f" [BRIDGE] Item used by entity {entity_id}")
    return jsonify(result)

@app.route('/api/entity/<int:entity_id>/inventory', methods=['GET'])
def get_inventory(entity_id):
    result = call_cpp('get_inventory', entity_id)
    return jsonify(result)

@app.route('/api/entity/<int:entity_id>/add-coins', methods=['POST'])
def add_coins(entity_id):
    data = request.json
    result = call_cpp('add_coins', entity_id, data.get('silver', 0), data.get('gold', 0))
    if result.get('status') == 'success':
        socketio.emit('coins_updated', result, broadcast=True)
    return jsonify(result)

@app.route('/api/entity/<int:entity_id>/remove-coins', methods=['POST'])
def remove_coins(entity_id):
    data = request.json
    result = call_cpp('remove_coins', entity_id, data.get('silver', 0), data.get('gold', 0))
    if result.get('status') == 'success':
        socketio.emit('coins_updated', result, broadcast=True)
    return jsonify(result)

@app.route('/api/game/state', methods=['GET'])
def get_game_state():
    result = call_cpp('get_game_state')
    return jsonify(result)

@app.route('/api/test', methods=['GET'])
def test():
    result = call_cpp('test_bridge')
    return jsonify(result)

@socketio.on('connect')
def on_connect():
    print(f"[SOCKET] Connected: {request.sid}")
    emit('connected', {'message': 'Connected to game server'})

@socketio.on('join_game')
def on_join_game(data):
    join_room(data.get('game_id'))
    emit('player_joined', data, room=data.get('game_id'))

if __name__ == '__main__':
    print("[BRIDGE] Starting on http://0.0.0.0:5000")
    socketio.run(app, host='0.0.0.0', port=5000, debug=True)