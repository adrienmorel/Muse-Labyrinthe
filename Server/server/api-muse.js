const express = require("express");
const check = require("check-types");

const Database = require("./db");
const Muse = require("./models/muse");

const Router = express.Router();

const moment = require("moment");


class ApiMuse {
    constructor(socket) {

        this.progress = 0;
        this.time = null;
        this.router = Router;
        this.router.get("/test", (req, res) => {

            var userID = 1;
            var partyID = 1;

            var that = this;
            var data = {};

            var progress;
            this.time = req.query.time || moment().format();

            try {
                progress  = req.query.progress;
                if(!progress) throw "";
            }catch (e) {
                res.send(that.makeError("NO_PROGRESS_ARGUMENT"))
            }
            data.progress = progress;
            //socket.io.sockets.emit('progress', progress);
            this.progress = progress;
            res.send(that.makeSuccess(data))

            var database = new Database();
            var muse;

            database
                .connectDB()
                .then(db => {
                    return db.collection("muse");
                })
                .then(musesCollection => {
                    muse = new Muse(musesCollection);
                    return muse.createMuse(userID, partyID, progress, this.time);
                })
                .then(newHistorique => {
                    database.closeDB();
                })
        });

        this.router.get("/result", (req, res) => {
            var that = this;
            var result = {
                progress : that.progress,
                time : that.time
            };
            res.send(result);
        });


    }

    makeError(errors) {
        var res = [];
        res[0] = { success: false, error: errors };
        return res;
    }

    makeSuccess(data) {
        var res = [];
        res[0] = { success: true, data: data };
        return res;
    }
}

module.exports = ApiMuse;
