const express = require('express');
const mongoose = require('mongoose');
const jwt = require('jsonwebtoken');
const User = mongoose.model('User');

const router = express.Router();
const UNPROCESSABLE_ENTITY = 422

//any time that someone makes a post request to '/signup' the function will run
router.post('/signup', async (req, res) => {
    const { email, password } = req.body;

    try {
        const user = new User({ email, password });
        await user.save();

        const token = jwt.sign({ userId: user._id }, 'YOUR_SECRET_KEY_HERE');
        res.send({ token });
    } catch (err) {
        return res.status(UNPROCESSABLE_ENTITY).send(err.message);
    }
    
});

router.post('/signin', async (req, res) => {
    const { email, password } = req.body;

    if (!email || !password) {
        return res.status(UNPROCESSABLE_ENTITY).send({ error: 'Must provide email and password' });
    }
    
    const user = await User.findOne({ email });
    if (!user) {
        return res.status(UNPROCESSABLE_ENTITY).send({ error: 'Invalid password or email' });
    }
    try {
        await user.comparePassword(password);
        const token = jwt.sign({ userId: user._id }, 'YOUR_SECRET_KEY_HERE');
        res.send({ token });
    } catch (err) {
        return res.status(UNPROCESSABLE_ENTITY).send({ error: 'Invalid password or email' });
    }
    
});


module.exports = router;