const WebSocket = require('ws');

const wss = new WebSocket.Server({ port: 81 });

wss.on('connection', (ws) => {
    console.log('Client connected');

    ws.on('message', (message) => {
        console.log('ESP32: %s', message);
        ws.send("Hello, ESP32");
    });
});
