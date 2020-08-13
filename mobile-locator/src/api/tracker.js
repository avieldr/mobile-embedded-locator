import axios from 'axios';
import { AsyncStorage } from 'react-native';

const instance =  axios.create({
    baseURL: 'https://a2a62d834470.ngrok.io' // IMPORTANT: while using Ngrok free version, this url is going to change each time we restart ngrok (expire after 8 hours) 
                                        // so this url needs to be update !!!
                                        // ngrok is opening up a direct connection from our express api and give us a public URL that we can have our mobile device to
                                        // look at, to get access to the api
});

instance.interceptors.request.use(
    async (config) => { // This function will be called on every request to the axios instance
        const token = await AsyncStorage.getItem('token');
        if (token) {
            config.headers.Authorization = `Bearer ${token}`;
        }
        return config;
    },
    (err) => {
        return Promise.reject(err);
    }
);

instance.interceptors.response.use(
    res => res,
    (err) => {
        console.log("interceptor error")
        return Promise.reject(err);
    }
);

export default instance;