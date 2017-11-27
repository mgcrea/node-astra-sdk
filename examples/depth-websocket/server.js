const path = require('path');
const express = require('express');
const socketIO = require('socket.io');
const http = require('http')

const app = express();
const server = http.Server(app);
const io = socketIO(server);

const NODE_PORT = process.env.NODE_PORT || 3000;
const NODE_HOST = process.env.NODE_HOST || '0.0.0.0';
const PUBLIC_PATH = path.resolve(__dirname, 'public');
const MODULES_PATH = path.resolve(__dirname, 'node_modules');

const log = console.log.bind(console);

server.listen(NODE_PORT, NODE_HOST, () => {
  const address = server.address();
  log('Express server listening on "%s:%d" ...', address.address, address.port);
});

app.use('/socket.io', express.static(path.join(MODULES_PATH, 'socket.io-client', 'dist')));
app.use(express.static('public'));

io.on('connection', function (socket) {
  log('Incoming websocket connection from client with id="%s"', socket.id);
});

exports.app = app;
exports.io = io;
