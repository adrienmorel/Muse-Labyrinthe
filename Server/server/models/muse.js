const ObjectID = require("mongodb").ObjectID;
const sanitize = require("mongo-sanitize");
const moment = require("moment");

class Muse {
    constructor(collection) {
        const that = this;
        that.collection = collection;
    }

    createMuse(userID, partyID, progress, time) {
        const that = this;
        return that.collection.insertOne({ userID: userID, partyID: partyID, progress: progress, time: time});
    }
    updateMuse(museID, filters, valuesToUpdate) {
        const that = this;
        if (!ObjectID.isValid(museID)) {
            return Promise.reject(new Error("INVALID_MUSE_ID_FORMAT"));
        }
        var myQuery = { _id: new ObjectID(museID) };
        Object.assign(myQuery, filters);
        if (Object.keys(valuesToUpdate).length === 0 && valuesToUpdate.constructor === Object) {
            return Promise.reject(new Error("NO_PROPERTY_WAS_PROVIDED_TO_UPDATE"));
        }
        return that.collection.findOneAndUpdate(myQuery, { $set: valuesToUpdate }, { returnOriginal: false });
    }

    deleteMuse(museID, filters) {
        const that = this;
        if (!ObjectID.isValid(museID)) {
            return Promise.reject(new Error("INVALID_MUSE_ID_FORMAT"));
        }
        var myQuery = { _id: new ObjectID(museID) };
        Object.assign(myQuery, filters);
        return that.collection.deleteOne(myQuery);
    }

    getMuseDataByUser(ofUserID) {
        const that = this;
        const query = { userID: ofUserID };
        return that.collection.find(query).toArray();
    }

    getMuseDataByParty(partyID) {
        const that = this;
        const query = { partyID: partyID };
        return that.collection.find(query).toArray();
    }

    getMusesThisWeek(ofUserID) {
        const that = this;
        const query = { userID: ofUserID , date : { $gt : moment().day(1).format()}};
        return that.collection.find(query).toArray();
    }

    getMusesThisMonth(ofUserID, since = null) {
        const that = this;
        const query = { userID: ofUserID , date : { $gt : moment().date(1).format()}};
        return that.collection.find(query).toArray();
    }

    checkMuse(museID, userID) {
        const that = this;
        if (!ObjectID.isValid(museID)) {
            return Promise.reject(new Error("INVALID_MUSE_ID_FORMAT"));
        }
        var myQuery = { _id: new ObjectID(museID), userID: userID };
        return that.collection.findOne(myQuery);
    }
}

module.exports = Muse;
