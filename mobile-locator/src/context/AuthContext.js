import { AsyncStorage } from 'react-native';
import createDataContext from './createDataContext';
import trackerApi from '../api/tracker';
import { navigate } from '../navigationRef';

const authReducer = (state, action) => {
    switch (action.type) {
        case 'add_error':
            return { ...state, errorMessage: action.payload };
        case 'signin':
            return { errorMessage: '', token: action.payload };
        case 'clear_error_message':
            return { ...state, errorMessage: '' };
        case 'signout':
            return { token: null, errorMessage: '' };
        default:
            return state;
    }
};

const tryLocalSignin = dispatch => async () => {
    const token = await AsyncStorage.getItem('token');
    if (token) {
        dispatch({ type: 'signin', payload: token });
        navigate('TrackList');
    } else {
        navigate('Signup');
    }
};

const clearErrorMessage = dispatch => () => {
    dispatch({ type: 'clear_error_message' });
}

const signup = dispatch => async ({ email, password }) => {
        // make api request to sign up with that email and password
        try {
            const response = await trackerApi.post('/signup', { email, password });
            console.log(response)
            await AsyncStorage.setItem('token', response.data.token);
            dispatch({ type: 'signin', payload: response.data.token });
            
            navigate('TrackList');
        } catch (err) {
            // console.log(err.message);
            // console.log(err.response.data);
            dispatch({ type: 'add_error', payload: 'Something went wrong with sign up' });
        }

        // if we sign up, modify our state, and say that we are authenticated 

        // if signing up fails, we probably need to reflect an error message somewhere
    }

const signin = (dispatch) => async ({ email, password }) => {
        // make api request to sign in with that email and password

        // if we sign in, modify our state, and say that we are authenticated 

        // if signing in fails, we probably need to reflect an error message somewhere

        try {
            const response = await trackerApi.post('/signin', { email, password });
            console.log(response)
            await AsyncStorage.setItem('token', response.data.token);
            dispatch({ type: 'signin', payload: response.data.token });
            
            navigate('TrackList');
        } catch (err) {
            dispatch({ type: 'add_error', payload: 'Something went wrong with sign in' });
        }
    }

const signout = (dispatch) => async () => {
    await AsyncStorage.removeItem('token');
    dispatch({ type: 'signout' });
    navigate('loginFlow');
};


export const { Provider, Context } = createDataContext(
    authReducer,
    { signin, signup, signout, clearErrorMessage, tryLocalSignin },
    { token: null, errorMessage: '' }
)