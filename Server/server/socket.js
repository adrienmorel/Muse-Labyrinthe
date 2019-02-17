const socketIO = require('socket.io');


class Socket {
    constructor(server) {
        this.io = socketIO(server);

        this.io.on('connection', (socket) => {
            console.log('New user connected');

            socket.on('message', (message) => {
                console.log(message);
            });

            socket.on('disconnect', () => {
                console.log('User disconnected')
            });
        });
    }
}

module.exports = Socket;