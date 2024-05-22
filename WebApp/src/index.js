// Import the functions need from the SDKs
import { initializeApp } from "https://www.gstatic.com/firebasejs/10.12.1/firebase-app.js";
import { getDatabase, ref, onValue } from "https://www.gstatic.com/firebasejs/10.12.1/firebase-database.js";

// TODO: Replace the following with your app's Firebase project configuration
// See: https://firebase.google.com/docs/web/learn-more#config-object
const firebaseConfig = {
  apiKey: "AIzaSyA6RyU5sX58C9uhyN1QYAvbMZhn8m3eP3Y",
  authDomain: "hivelink-abd1a.firebaseapp.com",
  databaseURL: "https://hivelink-abd1a-default-rtdb.asia-southeast1.firebasedatabase.app",
  projectId: "hivelink-abd1a",
  storageBucket: "hivelink-abd1a.appspot.com",
  messagingSenderId: "843058360587",
  appId: "1:843058360587:web:d1d90e47e657e2bb53320e",
  measurementId: "G-B31PD72N8G"
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);


// Initialize Realtime Database and get a reference to the service
const database = getDatabase(app);

// Replace 'a' with the actual user ID you want to fetch data for
const userRef = ref(database, 'Users/a');

onValue(userRef, (snapshot) => {
  const userData = snapshot.val();
  
  if (userData) {
    const temperature = userData.Temperature;
    const humidity = userData.Humidity;
    
    // Process the temperature and humidity values here
    console.log("Temperature:", temperature, "°C");
    console.log("Humidity:", humidity, "%");
  } else {
    console.log("No sensor data available for this user");
  }
});

