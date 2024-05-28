// Import the functions need from the SDKs
import { initializeApp } from "https://www.gstatic.com/firebasejs/10.12.1/firebase-app.js";
import { getDatabase, ref, query, orderByKey, limitToLast, onValue } from "https://www.gstatic.com/firebasejs/10.12.1/firebase-database.js";

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

// Initialize Realtime Database and get a reference
const database = getDatabase(app);

const dataContainer = document.getElementById('data-container');
const temperatureElement = document.getElementById('temperature');
const humidityElement = document.getElementById('humidity');

const hiveRef = ref(database, 'UsersData/Saijaiu1GKQl3clIFxLhI8Ce6Va2/Hive-01');


function updateData() {
  const latestDataQuery = query(hiveRef, orderByKey(), limitToLast(1));
  onValue(latestDataQuery, (snapshot) => {
    const data = snapshot.val();
    console.log('Data received from Firebase:', data); // Log received data
    if (data) {
      const latestEntry = Object.values(data)[0];
      console.log(`Temperature: ${latestEntry.temperature}°C`); // Debugging log
      temperatureElement.textContent = `Temperature: ${latestEntry.temperature}°C`;
      humidityElement.textContent = `Humidity: ${latestEntry.humidity}%`;
    } else {
      console.log('No data found');
      // Handle no data scenario (display message, etc.)
    }
  });
}

updateData();
setInterval(updateData, 10000); // Update data every 10 seconds
