function showGraphs(hiveRef) {
  let startDateStamp = null;
  let endDateStamp = null;
  const temperatureChart = initializeChart('temperatureChart', 'Temperature (°C)', 'rgba(255, 99, 132, 1)', 'rgba(255, 99, 132, 0.2)');
  const humidityChart = initializeChart('humidityChart', 'Humidity (%)', 'rgba(54, 162, 235, 1)', 'rgba(54, 162, 235, 0.2)');


  // Fetch and display the last 10 entries on initial load
  fetchLastEntries(10);

  // Event listener for start date picker
  document.getElementById('startDateTime').addEventListener('change', (event) => {
    let selectedDate = new Date(event.target.value);
    startDateStamp = Math.floor(selectedDate.getTime() / 1000); // Convert to Unix time (seconds)
    console.log("startDateStamp", startDateStamp)
  });

  // Event listener for end date picker
  document.getElementById('endDateTime').addEventListener('change', (event) => {
    let selectedDate = new Date(event.target.value);
    endDateStamp = Math.floor(selectedDate.getTime() / 1000); // Convert to Unix time (seconds)
    console.log("endDateStamp", endDateStamp)
  });

  // Event listener to the filter button
  document.getElementById('updateGraphs').addEventListener('click', () => fetchData(startDateStamp, endDateStamp));

  // Get the last entries
  function fetchLastEntries(limit) {
    const dataQuery = query(hiveRef, orderByKey(), limitToLast(limit));
    fetchAndDisplayData(dataQuery); 1717211640
  }

  function fetchData(startDateStamp, endDateStamp) {

    if (startDateStamp !== null && endDateStamp !== null) {
      const dataQuery = query(hiveRef, orderByKey(), startAt(startDateStamp.toString()), endAt(endDateStamp.toString())); // Fetch entries between startDateStamp and endDateStamp
      fetchAndDisplayData(dataQuery);
    } else {
      console.log("Please select both start and end dates.");
    }
  }

  function fetchAndDisplayData(dataQuery) {
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
        const formattedDate = getFormattedDate(date).toLocaleString(); // Format the date to a readable string
        timestamps.push(formattedDate); // Use formatted date as the label
        temperatures.push(entry.temperature);
        humidities.push(entry.humidity);
      });

      console.log('Timestamps:', timestamps);
      console.log('Temperatures:', temperatures);
      console.log('Humidities:', humidities);

      // Update the chart with new data
      updateChart(temperatureChart, timestamps, temperatures);
      updateChart(humidityChart, timestamps, humidities);
    });
  }
}

function initializeChart(canvasId, label, borderColor, backgroundColor, dataset) {
  const ctx = document.getElementById(canvasId).getContext('2d');
  return new Chart(ctx, {
    type: 'line', // Use 'line' chart type for time series data
    data: {
      labels: [],
      datasets: [
        {
          label: label,
          data: dataset,
          borderColor: borderColor, // Line color
          backgroundColor: backgroundColor, // Area below the line color
        }
      ]
    },
    options: {
      scales: {
        xAxes: [{ // Use 'x' for the x-axis
          type: 'time', // Enable time scale for x-axis
          time: {
            unit: 'minute', // Display timestamps in minutes (adjust as needed)
            stepSize: 10, // Show a data point every 10 minutes
          },
          title: {
            display: true,
            text: 'Time'
          }
        }],
        yAxes: [{ // Use 'y' for the y-axis
          title: {
            display: true,
            text: label
          }
        }]
      }
    }
  });
}

function updateChart(chart, labels, data) {// labels are the timestamps
  chart.data.labels = labels;
  chart.data.datasets[0].data = data;
  chart.options.scales.y.ticks.suggestedMin = Math.min(...data) - 1; // Set minimum y-axis value slightly below min temperature
  chart.options.scales.y.ticks.suggestedMax = Math.max(...data) + 1; // Set maximum y-axis value slightly above max temperature
  chart.update();
}