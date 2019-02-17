const express = require("express");
var bodyParser = require("body-parser");
const apiMuse = require("./api-muse");
const http = require('http');
const Socket = require('./socket');
const fs = require("fs");
const path = require("path");

class Server {
    constructor() {
        this.app = express();

        this.app.use(bodyParser.json());
        this.app.use(
            bodyParser.urlencoded({
                extended: true
            })
        );



        // serve default page
        this.app.get("/", (req, res) => {
            res.send(
                fs.readFileSync(
                    path.join(
                        process.cwd(),
                        `server/public/views/index.html`
                    ),
                    "utf8"
                )
            );
        });

        // serve default page
        this.app.get("/socket", (req, res) => {
            res.send(
                fs.readFileSync(
                    path.join(
                        process.cwd(),
                        `server/public/views/socket.html`
                    ),
                    "utf8"
                )
            );
        });


        this.app.use('/public', express.static(__dirname + '/public'));

        this.server = http.createServer(this.app);

        this.server.on('connexion', (sokcet) =>{
            sokcet.on();
        })

        this.socket = new Socket(this.server);

        this.apiMuse = new apiMuse(this.socket);

        this.app.use("/muse", this.apiMuse.router);
    }

    start(port) {
        this.server.listen(port, () => {
            console.log(`Listening on the port ${port}...`);
        });
    }
}

module.exports = new Server();
