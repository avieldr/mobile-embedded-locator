const jwt = require('jsonwebtoken');
const mongoose = require('mongoose');
const User = mongoose.model('User');

// this makes sure that every http request that comes to us must contain a header with the jws token
const STATUS_UNAUTHORIZED = 401

module.exports = (req, res, next) => {
    const { authorization } = req.headers;
    // authorization === 'Bearer kjakfjasdfbhjubsd...'

    if (!authorization) {
        return res.status(STATUS_UNAUTHORIZED).send({ error: 'Login required.' });
    }

    const token = authorization.replace('Bearer ', '');
    jwt.verify(token, 'YOUR_SECRET_KEY_HERE', async (err, payload) => {
        if (err) {
            return res.status(STATUS_UNAUTHORIZED).send({ error: 'Login required.' });
        }

        const { userId } = payload;

        const user = await User.findById(userId);
        req.user = user;
        next();
    });
};