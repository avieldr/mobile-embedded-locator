import React, { useContext } from 'react';
import { View, StyleSheet, Text } from 'react-native';
import { NavigationEvents } from 'react-navigation';
import AuthForm from '../components/AuthForm';
import { Context as AuthContext } from '../context/AuthContext'; 

import NavLink from '../components/navLink';

const SigninScreen = () => {
    const { state, signin, clearErrorMessage } = useContext(AuthContext);

    return <View style={styles.container}>
    <NavigationEvents 
        // onWillFocus={() => {}}
        // onDidFocus={() => {}}
        onWillBlur={clearErrorMessage}
        // onDidBlur={() => {}}
    />
    <AuthForm 
        onSubmit={signin}
        headerText="Sign In to Your Account"
        errorMessage={state.errorMessage}
        submitButtonText="Sign In"
    />

    <NavLink 
        text="Dont have an account? Sign up instead"
        routeName="Signup"
    />
    
    
</View>
};

SigninScreen.navigationOptions = () => {
    return {
        header: null
    };
};

const styles = StyleSheet.create({
    container: {
        flex: 1,
        justifyContent: 'center',
        marginBottom: 200
    }
});

export default SigninScreen;