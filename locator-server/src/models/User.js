const mongoose = require('mongoose');
const bcrypt = require('bcrypt');

const userSchema = new mongoose.Schema({
    email: {
        type: String,
        unique: true,
        required: true
    },
    password: {
        type: String,
        required: true
    }
});

// a hook that will be called before any attempt to save a userSchema instance.
// we use function() instead of () => {} because we want 'this' keyword to refer to the user
// with arrow function 'this' will refer to the context inside the file
userSchema.pre('save', function(next) {
    const user = this;
    if (!user.isModified('password')) {
        return next();
    }

    bcrypt.genSalt(10, (err, salt) => {
        if (err) {
            return next(err);
        }

        bcrypt.hash(user.password, salt, (err, hash) => {
            if (err) {
                return next(err);
            }

            user.password = hash;
            next();
        })  
    });
});

userSchema.methods.comparePassword = function comparePassword(candidatePassword) {

    return new Promise((resolve, reject) => {
        bcrypt.compare(candidatePassword, this.password, (err, isMatch) => {
            if (err) {
                return reject(err);
            }

            if (!isMatch) {
                return reject(false);
            }

            resolve(true);
        });
    });
}

mongoose.model('User', userSchema);