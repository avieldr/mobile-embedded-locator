import React, { useEffect, useState, useContext } from 'react';
import { requestPermissionsAsync, watchPositionAsync, Accuracy } from 'expo-location';
import { Context as LocationContext } from '../context/LocationContext';


export default (shouldTrack, callback) => {
    const { state: { last_encountered }, getLastLocation } = useContext(LocationContext);
    const [err, setErr] = useState(null);

    useEffect(() => {
        let subscriber;
        const startWatching = async () => {
            try {
                subscriber = setInterval(() => {
                    const lastLocation = getLastLocation();
                }, 3000);
            } catch (err) {
                console.log(err.message);
                // setErr(err);
            }
        };

        if (shouldTrack) {
            startWatching();
        } else {
            if (subscriber) {
                clearInterval(subscriber);
            }
            subscriber = null;
        }

        return () => {
            if (subscriber) {
                clearInterval(subscriber);
            }
        }
    }, [shouldTrack, callback]); // if shouldTrack changes, react will run useEffect again

    return [err];
};