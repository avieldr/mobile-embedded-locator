
import React, { useContext, useState } from 'react';
import { View, StyleSheet, Text, FlatList, TouchableOpacity, Button } from 'react-native';
import { NavigationEvents } from 'react-navigation';
import { ListItem } from 'react-native-elements'; 
import { Context as LocationContext } from '../context/LocationContext';
import Spacer from '../components/Spacer';
import { FontAwesome } from '@expo/vector-icons';
import trackerApi from '../api/tracker'

import DateTimePickerModal from "react-native-modal-datetime-picker";



const OptionsScreen = ({ navigation }) => {
    const { state, getLocationsList } = useContext(LocationContext);
    
    
    const [dateState, setDateState] = useState({ startDate: undefined, endDate: undefined, toSet: 0, modalIsVisible: false });

 
  const hideDatePicker = () => {
    setDateState({ ...dateState, modalIsVisible: false })
  };
 
  const handleConfirm = date => {
      switch (dateState.toSet) {
        case 0:
            setDateState({ ...dateState, startDate: date, modalIsVisible: false });
            return;
        case 1:
            setDateState({ ...dateState, endDate: date, modalIsVisible: false });
            return;
        default:
            return;
        
      }
    
  };

    return <View>
    <Spacer />
    <NavigationEvents onWillBlur={() => {
        // fetchTracks("555", "777")
    }} /> 
    <View>
      <DateTimePickerModal
        isVisible={dateState.modalIsVisible}
        mode="date"
        onConfirm={handleConfirm}
        onCancel={hideDatePicker}
      />
    </View>

    <Spacer>
    <View>
        <TouchableOpacity onPress={() => {
            setDateState({ ...dateState, toSet: 0, modalIsVisible: true });
        }}>
            <Text>From: {dateState.startDate ? dateState.startDate.toString() : "Plese select..."}</Text>
        </TouchableOpacity>
    </View>
    </Spacer>

        <Spacer>
    <View>
        <TouchableOpacity onPress={() => {
            setDateState({ ...dateState, toSet: 1, modalIsVisible: true });
        }}>
            <Text>To: {dateState.endDate ? dateState.endDate.toString() : "Plese select..."}</Text>
        </TouchableOpacity>
    </View>
    </Spacer>

        <Spacer>
    <Button title="get locations" onPress={async () => {
        
        await getLocationsList(dateState.startDate, dateState.endDate);
        navigation.navigate('TrackCreate');
    }}/>
    </Spacer>
       
    </View>
}

OptionsScreen.navigationOptions = {
    title: 'History',
    tabBarIcon: <FontAwesome name="th-list" size={20} />,
    
};


const styles = StyleSheet.create({

});

export default OptionsScreen;