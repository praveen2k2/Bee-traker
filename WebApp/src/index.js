// Import the functions need from the SDKs
import { initializeApp } from "firebase/app";
import { getAuth, onAuthStateChanged } from "firebase/auth";
import { getFirestore } from "firebase/firestore";

// Your web app's Firebase configuration
// For Firebase JS SDK v7.20.0 and later, measurementId is optional
const firebaseApp = initializeApp({
    apiKey: "AIzaSyA6RyU5sX58C9uhyN1QYAvbMZhn8m3eP3Y",
    authDomain: "hivelink-abd1a.firebaseapp.com",
    databaseURL: "https://hivelink-abd1a-default-rtdb.asia-southeast1.firebasedatabase.app",
    projectId: "hivelink-abd1a",
    storageBucket: "hivelink-abd1a.appspot.com",
    messagingSenderId: "843058360587",
    appId: "1:843058360587:web:d1d90e47e657e2bb53320e",
    measurementId: "G-B31PD72N8G"
});

const auth = getAuth(firebaseApp);
const db = getFirestore(firebaseApp);
