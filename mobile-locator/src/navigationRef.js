// allows us to navigate inside the application, outside from a react component

import { NavigationActions } from 'react-navigation';

let navigator;

export const setNavigator = (nav) => {
    navigator = nav;
};

export const navigate = (routeName, params) => {
    navigator.dispatch(
        NavigationActions.navigate({
            routeName,
            params
        })
    );
};