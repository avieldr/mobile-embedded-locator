// import '../_mockLocation';

import React, { useContext, useCallback, useState } from 'react';
import { StyleSheet } from 'react-native';
import { Text } from 'react-native-elements';
import { SafeAreaView, withNavigationFocus } from 'react-navigation';
import Map from '../components/Map';
import { Context as LocationContext } from '../context/LocationContext';
import useLocation from '../hooks/useLocation';
import { FontAwesome } from '@expo/vector-icons';

const MapScreen = ({ isFocused }) => {
    const { state: { recording } } = useContext(LocationContext);

    const callback = useCallback(location => {
        // addLocation(location, recording);
    }, [recording]);
    const [err] = useLocation(isFocused || recording, callback);

    return <SafeAreaView forceInset={{ top: 'always' }}>
        <Text h2>Real-Time Location</Text>
        <Map />
        {err ? <Text h2>Please enable location services</Text> : null}

        
        
    </SafeAreaView> 
    
}

MapScreen.navigationOptions = {
    title: 'Tracker',
    tabBarIcon: <FontAwesome name="location-arrow" size={20} />
}

const styles = StyleSheet.create({

});

export default withNavigationFocus(MapScreen);