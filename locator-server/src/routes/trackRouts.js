const express = require('express');
const mongoose = require('mongoose');
const authRequire = require('../middlewares/authRequire');

const Track = mongoose.model('Track');

const UNPROCESSABLE_ENTITY = 422
const router = express.Router();

router.use(authRequire);

router.get('/lastLocation', async (req, res) => {


    const tracks = await Track.find({ userId: req.user._id});
    // console.log(tracks);

    const latestLocationTimestamp = Math.max.apply(null, tracks.map(function(e) {
        return e.createdAt;
      }));

    res.send(tracks.find(loc => loc.createdAt === latestLocationTimestamp));
});

router.get('/tracks', async (req, res) => {

    const tracks = await Track.find({ userId: req.user._id });

    res.send(tracks);
});

router.post('/tracks', async (req, res) => {


    const { name, locations } = req.body;

    if (!name || !locations) {
        return res
        .status(UNPROCESSABLE_ENTITY)
        .send({ error: 'Please provide a name and locations' });
    }

    try {
        locations.coords.latitude = locations.coords.latitude // / 1e7;
        locations.coords.longitude = locations.coords.longitude // / 1e7;
        const track = new Track({ name, locations: locations, userId: req.user._id, createdAt: Date.now() });
    await track.save();
    res.send(track);
    } catch (err) {

        res.status(UNPROCESSABLE_ENTITY).send({ error: err.message });
    }
    
});

module.exports = router;