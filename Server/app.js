const Server = require("./server/server");
const port = process.env.PORT || 8080;

console.log("PORT CONNEXION : ", port)
Server.start(port);