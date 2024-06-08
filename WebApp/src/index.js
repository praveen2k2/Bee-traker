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
      temperatureElement.textContent = `${latestEntry.temperature}°C`;
      humidityElement.textContent = `${latestEntry.humidity}%`;
    } else {
      console.log('No data found');
      // Handle no data scenario (display message, etc.)
    }
  });
}

function showGraphs() {
  const dataQuery = query(hiveRef, orderByKey(), limitToLast(10)); // Fetch 10 recent entries
  onValue(dataQuery, (snapshot) => {
    const data = snapshot.val();
    console.log('Data received from Firebase:', data);

    if (!data) {
      console.error("No data available for graphs");
      return;
    }

    const timestamps = [];
    const temperatures = [];
    const humidities = [];

    // Loop through all entries in the data object
    Object.entries(data).forEach(([key, entry]) => {
      const date = new Date(Number(key)); // Convert timestamp string to number, then to Date
      const formattedDate = date.toLocaleString(); // Format the date to a readable string
      timestamps.push(formattedDate); // Use formatted date as the label
      temperatures.push(entry.temperature);
      humidities.push(entry.humidity);
    });

    // Create the temperature chart configuration
    const ctxTemp = document.getElementById('temperatureChart').getContext('2d');
    new Chart(ctxTemp, {
      type: 'line', // Use 'line' chart type for time series data
      data: {
        labels: timestamps,
        datasets: [
          {
            label: 'Temperature (°C)',
            data: temperatures,
            borderColor: 'rgba(255, 99, 132, 1)', // Red color for temperature line
            backgroundColor: 'rgba(255, 99, 132, 0.2)', // Transparent red for area below the line
          }
        ]
      },
      options: {
        scales: {
          xAxes: [{
            type: 'time', // Enable time scale for x-axis
            time: {
              unit: 'minute', // Display timestamps in minutes (adjust as needed)
              unitStepSize: 10, // Show a data point every 10 minutes
            }
          }],
          yAxes: [{
            ticks: {
              suggestedMin: Math.min(...temperatures) - 1, // Set minimum y-axis value slightly below min temperature
              suggestedMax: Math.max(...temperatures) + 1, // Set maximum y-axis value slightly above max temperature
            }
          }]
        }
      }
    });

    // Create the humidity chart configuration
    const ctxHum = document.getElementById('humidityChart').getContext('2d');
    new Chart(ctxHum, {
      type: 'line', // Use 'line' chart type for time series data
      data: {
        labels: timestamps,
        datasets: [
          {
            label: 'Humidity (%)',
            data: humidities,
            borderColor: 'rgba(54, 162, 235, 1)', // Blue color for humidity line
            backgroundColor: 'rgba(54, 162, 235, 0.2)', // Transparent blue for area below the line
          }
        ]
      },
      options: {
        scales: {
          xAxes: [{
            type: 'time', // Enable time scale for x-axis
            time: {
              unit: 'minute', // Display timestamps in minutes (adjust as needed)
              unitStepSize: 10, // Show a data point every 10 minutes
            }
          }],
          yAxes: [{
            ticks: {
              suggestedMin: Math.min(...humidities) - 1, // Set minimum y-axis value slightly below min humidity
              suggestedMax: Math.max(...humidities) + 1, // Set maximum y-axis value slightly above max humidity
            }
          }]
        }
      }
    });
  });
}


function checkHiveConditions() {
  const dataQuery = query(hiveRef, orderByKey(), limitToLast(1)); // Fetch recent entries
  onValue(dataQuery, (snapshot) => {
    const data = snapshot.val();
    console.log('Data received from Firebase:', data);

    if (!data) {
      console.error("No data available for updates");
      return;
    }

    const updatesContainer = document.querySelector('.updates');
    updatesContainer.innerHTML = ''; // Clear existing updates

    // Loop through all entries in the data object
    Object.entries(data).forEach(([key, entry]) => {
      const timestamp = new Date(Number(key));
      const timeAgo = timeSince(timestamp);
      const messages = generateMessages(entry);

      messages.forEach(message => {
        const updateElement = document.createElement('div');
        updateElement.classList.add('update');
        updateElement.innerHTML = `
            <div class="profile-photo">
              <img src="path/to/profile/photo.png" alt="Profile Photo">
            </div>
            <div class="message">
              <p><b>${message.title}</b>: ${message.body}</p>
              <small class="text-muted">${timeAgo}</small>
            </div>
          `;
        updateElement.addEventListener('click', () => {
          window.location.href = `details.html?hive=${message.hiveId}&timestamp=${key}`;
        });
        updatesContainer.appendChild(updateElement);
      });
    });
  });
}

function generateMessages(entry) {
  const messages = [];
  const idealTemp = 28;
  if (entry.temperature < idealTemp) {
    messages.push({
      hiveId: entry.hiveId,
      title: 'Hive 1',
      //title: `Hive ${entry.hiveId}`,
      body: 'Temperature is below.',
    });
  }
  // Add other conditions and messages as needed
  return messages;
}

function timeSince(date) {
  const seconds = Math.floor((new Date() - date) / 1000);
  let interval = Math.floor(seconds / 31536000);
  if (interval > 1) return interval + ' years ago';
  interval = Math.floor(seconds / 2592000);
  if (interval > 1) return interval + ' months ago';
  interval = Math.floor(seconds / 86400);
  if (interval > 1) return interval + ' days ago';
  interval = Math.floor(seconds / 3600);
  if (interval > 1) return interval + ' hours ago';
  interval = Math.floor(seconds / 60);
  if (interval > 1) return interval + ' minutes ago';
  return Math.floor(seconds) + ' seconds ago';
}

// Call the function to check hive conditions and show messages
checkHiveConditions();



updateData();
showGraphs();
setInterval(updateData, 10000); // Update data every 10 seconds
