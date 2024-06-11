// Import the functions need from the SDKs
import { initializeApp } from "https://www.gstatic.com/firebasejs/10.12.1/firebase-app.js";
import { getDatabase, ref, update, query, orderByKey, limitToLast, onValue } from "https://www.gstatic.com/firebasejs/10.12.1/firebase-database.js";
import { getAuth, onAuthStateChanged, signOut } from "https://www.gstatic.com/firebasejs/10.12.1/firebase-auth.js";


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
const auth = getAuth(app);
const user = auth.currentUser;
// Initialize Realtime Database and get a reference
const database = getDatabase(app);

const temperatureElement = document.getElementById('temperature');
const humidityElement = document.getElementById('humidity');

// const hiveRef = ref(database, 'UsersData/Saijaiu1GKQl3clIFxLhI8Ce6Va2/Hive-01');

onAuthStateChanged(auth, (user) => {
  if (user) {
    // User is signed in, see docs for a list of available properties
    // https://firebase.google.com/docs/reference/js/auth.user
    const uid = user.uid;
    const hiveRef = ref(database, 'UsersData/' + uid + '/Hive-01');
    checkHiveConditions(hiveRef);
    loadWifiInfo()
    updateData(hiveRef);
    showGraphs(hiveRef);
    setInterval(updateData, 10000); // Update data every 10 seconds
    // ...
  } else {
    // User is signed out
    // ...
  }
});

const signOutButton = document.getElementById('sign-out'); // Change signUp to sign-out
signOutButton.addEventListener("click", function () {
  const auth = getAuth();
  signOut(auth).then(() => {
    window.location.replace('../auth/');
  }).catch((error) => {
    // An error happened.
  });
});

function updateData(hiveRef) {
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

function showGraphs(hiveRef) {
  const dataQuery = query(hiveRef, orderByKey(), limitToLast(24)); // Fetch 10 recent entries
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
      const date = new Date(Number(key) * 1000); // Convert timestamp string to number, then to Date
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
              unit: 'month', // Display data points every day (adjust as needed)
              tooltipFormat: 'MM-DD', // Format for tooltip (optional)
              displayFormats: { // Format for x-axis labels
                day: 'DD',
                month: 'MM'
              }
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

const messagesTable = document.getElementById('messagesTable');

function checkHiveConditions(hiveRef) {
  const dataQuery = query(hiveRef, orderByKey(), limitToLast(24)); // Fetch recent entries
  onValue(dataQuery, (snapshot) => {
    const data = snapshot.val();
    console.log('Data received from Firebase:', data);

    if (!data) {
      console.error("No data available for updates");
      return;
    }

    const updatesContainer = document.querySelector('.updates');
    updatesContainer.innerHTML = ''; // Clear existing updates
    const messagesTable = document.getElementById('messagesTable');
    messagesTable.innerHTML = ''; // Clear existing table entries

    // Create an array to hold all messages
    let allMessages = [];

    // Loop through all entries in the data object
    Object.entries(data).forEach(([key, entry]) => {
      const timestamp = new Date(Number(key) * 1000);
      const timeAgo = timeSince(timestamp);
      const messages = generateMessages(entry);

      messages.forEach(message => {
        allMessages.push({
          key,
          timeAgo,
          ...message
        });
      });
    });

    // Sort messages by timestamp and get the latest 3
    const latestMessages = allMessages.slice(-3).reverse();

    // Display the latest 3 messages in the updates container
    latestMessages.forEach(({ key, timeAgo, icon, title, short }) => {
      const updateElement = document.createElement('div');
      updateElement.classList.add('update');
      updateElement.innerHTML =
        `
        <div class="update">
            <div class="message-icon ${icon}">
                <span class="material-symbols-outlined">${icon}</span>
            </div>
            <div class="message">
                <p><b>${title}</b>: ${short}</p>
                <small class="text-muted">${timeAgo}</small>
            </div>
        </div>
        `;
      updateElement.addEventListener('click', () => {
        window.location.href = `#messagesTable`;
        // Scroll to the messages table and highlight the corresponding message
        const targetRow = document.getElementById(`message-${key}`);
        if (targetRow) {
          targetRow.scrollIntoView({ behavior: 'smooth' });
          targetRow.classList.add('highlight');
          setTimeout(() => {
            targetRow.classList.remove('highlight');
          }, 2000);
        }
      });
      updatesContainer.appendChild(updateElement);
    });

    // Display all messages in the table
    allMessages.forEach(({ key, timeAgo, icon, hiveId, body }) => {
      const tableRow = document.createElement('tr');
      tableRow.id = `message-${key}`;
      tableRow.innerHTML = `
        <td class="message-icon ${icon}"><span class="material-symbols-outlined">${icon}</span></td>
        <td>${hiveId}</td>
        <td>${timeAgo}</td>
        <td class="primary">${body}</td>
      `;
      messagesTable.appendChild(tableRow);
    });
  });
}


function generateMessages(entry) {
  const messages = [];
  const lowTemp = 32;
  const highTemp = 35;
  const lowHumidity = 20;
  const highHumidity = 40;
  const weightLoss = 10;

  if (entry.temperature < lowTemp) {
    messages.push({
      hiveId: entry.hiveId,
      icon: `severe_cold`,
      title: `Hive ${entry.hiveId}`,
      short: `Temperature drop`,
      body: `Low temperature of ${entry.temperature}⁰C reported from the hive.`,
    });
  }
  if (entry.temperature > highTemp) {
    messages.push({
      hiveId: entry.hiveId,
      icon: `emergency_heat`,
      title: `Hive ${entry.hiveId}`,
      short: `Temperature increase`,
      body: `High temperature of ${entry.temperature}⁰C reported from the hive.`,
    });
  }
  if (entry.humidity < lowHumidity) {
    messages.push({
      hiveId: entry.hiveId,
      icon: `cool_to_dry`,
      title: `Hive ${entry.hiveId}`,
      short: `Humidity drop`,
      body: `Low humidity of ${entry.humidity}% reported from the hive.`,
    });
  }
  if (entry.humidity > highHumidity) {
    messages.push({
      hiveId: entry.hiveId,
      icon: `emergency_heat_2`,
      title: `Hive ${entry.hiveId}`,
      short: `Humidity increase`,
      body: `High humidity of ${entry.humidity}% reported from the hive.`,
    });
  }
  if (entry.weight < weightLoss) {
    messages.push({
      hiveId: entry.hiveId,
      icon: `monitor_weight_loss`,
      title: `Hive ${entry.hiveId}`,
      short: `Weight drop`,
      body: `Weight loss detected in the hive.`,
    });
  }
  if (entry.feedBees) {
    messages.push({
      hiveId: entry.hiveId,
      icon: `food_bank`,
      title: `Hive ${entry.hiveId}`,
      short: `Feeding time`,
      body: `Time to feed the bees.`,
    });
  }
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



// Profile updates
const updateInfo = document.getElementById('update');
const ssid = document.getElementById('ssid');
const wifiPassword = document.getElementById('wifiPassword');
const email = document.getElementById('email');
const password = document.getElementById('password');

function updateProfileInfo(event) {
  event.preventDefault(); // Prevent form submission and page refresh
  if (auth.currentUser) {
    const uid = auth.currentUser.uid;
    const UserRef = ref(database, 'UsersData/' + uid + '/ProfileInfo');

    const info = {
      ssid: ssid.value,
      wifiPassword: wifiPassword.value,
      email: email.value,
      password: password.value
    };

    update(UserRef, info)
      .then(() => {
        alert('WiFi information updated successfully.');
      })
      .catch((error) => {
        alert('Error updating WiFi information:', error);
      });
  }
}

function loadWifiInfo() {
  if (auth.currentUser) {
    const uid = auth.currentUser.uid;
    const UserRef = ref(database, 'UsersData/' + uid + '/ProfileInfo');

    onValue(UserRef, (snapshot) => {
      const data = snapshot.val();
      if (data) {
        email.value = data.email || '';
        password.value = data.password || '';
        ssid.value = data.ssid || '';
        wifiPassword.value = data.wifiPassword || '';
      }
    }, {
      onlyOnce: true
    });
  }
}

updateInfo.addEventListener('click', updateProfileInfo);


// Adding Hives
document.addEventListener('DOMContentLoaded', () => {
  const addHiveButton = document.querySelector('.add-hive');
  const popover = document.getElementById('popover');
  const addHiveButtonPopover = document.getElementById('addHiveButton');

  addHiveButton.addEventListener('click', () => {
    const rect = addHiveButton.getBoundingClientRect();
    popover.style.top = `${rect.bottom + window.scrollY}px`;
    popover.style.left = `${rect.left + window.scrollX}px`;
    popover.style.display = 'block';
  });

  addHiveButtonPopover.addEventListener('click', () => {
    const hiveName = document.getElementById('newHiveName').value;
    const beeCount = document.getElementById('newHiveBeeCount').value;

    if (hiveName && beeCount) {
      addHive(hiveName, beeCount);
      popover.style.display = 'none';
      document.getElementById('newHiveName').value = '';
      document.getElementById('newHiveBeeCount').value = '';
    } else {
      alert('Please fill in both fields');
    }
  });

  function addHive(name, count) {
    const hivesContainer = document.querySelector('.my-hives');
    const newHive = document.createElement('div');
    newHive.className = 'hive';

    newHive.innerHTML = `
          <div class="icon">
              <span class="material-icons-sharp">hive</span>
          </div>
          <div class="right">
              <div class="info">
                  <h3>${name}</h3>
              </div>
              <h3 class="total-bee-count">${count}</h3>
              <span class="material-icons-sharp delete-btn">delete</span>
          </div>
      `;

    hivesContainer.insertBefore(newHive, hivesContainer.querySelector('.add-hive'));

    // Add delete event listener to the new hive
    newHive.querySelector('.delete-btn').addEventListener('click', () => {
      hivesContainer.removeChild(newHive);
    });
  }

  // Add delete event listeners to existing hives
  document.querySelectorAll('.hive .delete-btn').forEach(deleteBtn => {
    deleteBtn.addEventListener('click', (event) => {
      const hive = event.target.closest('.hive');
      hive.parentNode.removeChild(hive);
    });
  });

  // Hide the popover when clicking outside of it
  document.addEventListener('click', (event) => {
    if (!popover.contains(event.target) && !addHiveButton.contains(event.target)) {
      popover.style.display = 'none';
    }
  });
});
