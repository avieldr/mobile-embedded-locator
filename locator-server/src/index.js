const { mongoUri } = require('../config');

require('./models/User');
require('./models/Track');
const express = require('express');
const mongoose = require('mongoose');
const bodyParser = require('body-parser');
const authRoutes = require('./routes/authRoutes');
const trackRoutes = require('./routes/trackRouts');
const authRequire = require('./middlewares/authRequire');

const app = express();

app.use(bodyParser.json()) // must be located before next line
app.use(authRoutes);       //
app.use(trackRoutes);

// connect app to the DB
mongoose.connect(mongoUri, {
    useNewUrlParser: true,
    useCreateIndex: true,
    useUnifiedTopology: true
});
// any time we successfully connect to the mongoose instance the callback function will be called
mongoose.connection.on('connected', () => {
    console.log('Connected successfully to your mongo instance');
});
// error callback function
mongoose.connection.on('error', (err) => {
    console.error('Error connecting to mongo', err);
});

// this function will be called when a get http request arrives to the root route of our app
// arguments are request and response
app.get('/', authRequire, (req, res) => {
    res.send(`Your email: ${req.user.email}`);
});


// to activate and check the listener run: node src/index.js
app.listen(3000, () => {
    console.log("Listening on http://localhost:3000/");
});