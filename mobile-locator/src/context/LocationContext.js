import createDataContext from './createDataContext';
import trackerApi from '../api/tracker';

const locationReducer = (state, action) => {
    switch (action.type) {
        case 'add_current_location':
            return { ...state, currentLocation: action.payload };
        case 'add_location':
            const alreadyExist = state.locations.find(loc => loc.createdAt === action.payload.createdAt);
            return alreadyExist ? state : { ...state, locations: [...state.locations, action.payload] };
        case 'set_locations':
            return { ...state, locations: action.payload}
        case 'reset':
            return { ...state, locations: [] };
        default:
            return state;
    }
};


const getLastLocation = dispatch => async () => {
    const response = await trackerApi.get('/lastLocation');
    const loc = response.data;
    // console.log(response.data);
    dispatch({ type: 'add_current_location', payload: loc });

    // dispatch({ type: 'add_location', payload: loc })
};

const getLocationsList = dispatch => async (startTime, endTime) => {

    const now = new Date();
    var d = new Date();
    d.setDate(d.getDate()-5);
    // console.log(now.getTime() / 1000, d.getTime() / 1000);


    const response = await trackerApi.get('/tracks');
    const loc = response.data;
    const loca = response.data.filter(loc => (loc.createdAt / 1000 <= endTime / 1000 && loc.createdAt / 1000 > startTime / 1000));
    // console.log("currLocation:", loca);
    dispatch({ type: 'set_locations', payload: loca });

    // dispatch({ type: 'add_location', payload: loc })
};

export const { Context, Provider } = createDataContext(
    locationReducer,
    { getLastLocation, getLocationsList },
    { locations: [], currentLocation: null }
);