const mongoose = require('mongoose');

const pointSchema = new mongoose.Schema({
    coords: {
        latitude: Number,
        longitude: Number,
    }
});

const trackSchema = new mongoose.Schema({
    userId: {
        type: mongoose.Schema.Types.ObjectId,
        ref: 'User'
    },
    name: {
        type: String,
        default: ''
    },
    locations: pointSchema,
    createdAt: Number,
    
});

mongoose.model('Track', trackSchema);