import React, { useContext, useState } from 'react';
import  { Text, StyleSheet, ActivityIndicator, View } from 'react-native';
import MapView, { Polyline, Circle } from 'react-native-maps';
import { Context as LocationContext } from '../context/LocationContext';
import { CheckBox } from 'react-native-elements';


const Map = () => {
    const { state: { currentLocation, locations } } = useContext(LocationContext);
    const [mapState, setMapState] = useState({ autoFocus: true });


    if (!currentLocation) {
        return <ActivityIndicator size="large" style={{ marginTop: 200 }}/>
    }

    var d = new Date(0); // The 0 there is the key, which sets the date to the epoch
    d.setUTCSeconds(parseInt(currentLocation.createdAt)/ 1000);
    
    return <View>
        <MapView 
        style={styles.map}
        initialRegion={{
            ...currentLocation.locations.coords,
            latitudeDelta: 0.01,
            longitudeDelta: 0.01
        }}
        region={mapState.autoFocus
         ? {
            ...currentLocation.locations.coords,
            latitudeDelta: 0.01,
            longitudeDelta: 0.01
        }
        : null}
        
    >
    <Circle 
        center={currentLocation.locations.coords}
        radius={40}
        strokeColor="rgba(158,158,255, 1.0)"
        fillColor="rgba(158,158,255, 0.3)"
    />
    <Polyline coordinates={locations.map(loc => loc.locations.coords)}/>
    </MapView>
    <CheckBox title="Auto Focus" checked={mapState.autoFocus} onPress={() => {
            setMapState({ autoFocus: !mapState.autoFocus })
        }} />
    <View style={{ paddingHorizontal: 20}}>
        <Text>Last Updated At:{'\r\n'} {d.toString()}</Text>
    </View>
        
    </View>
    
    
};

const styles = StyleSheet.create({
    map: {
        height: 300
    }
});

export default Map;