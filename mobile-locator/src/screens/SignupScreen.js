import React, { useContext } from 'react';
import { View, StyleSheet } from 'react-native';
import { NavigationEvents } from 'react-navigation';

import { Context as AuthContext } from '../context/AuthContext'; 
import AuthForm from '../components/AuthForm';
import NavLink from '../components/navLink';

const SignupScreen = ({ navigation }) => {
    const { state, signup, clearErrorMessage } = useContext(AuthContext);

    

    return <View style={styles.container}>
        <NavigationEvents 
        // onWillFocus={() => {}}
        // onDidFocus={() => {}}
        onWillBlur={clearErrorMessage}
        // onDidBlur={() => {}}
        />
        <AuthForm 
            onSubmit={signup}
            headerText="Sign Up for Tracker"
            errorMessage={state.errorMessage}
            submitButtonText="Sign Up"
        />

        <NavLink 
            text="Already have an account? Sign in instead"
            routeName="Signin"
        />
        
        
    </View>
};

SignupScreen.navigationOptions = () => {
    return {
        header: null
    };
};

const styles = StyleSheet.create({
    container: {
        flex: 1,
        justifyContent: 'center',
        marginBottom: 250
    }
});

export default SignupScreen;