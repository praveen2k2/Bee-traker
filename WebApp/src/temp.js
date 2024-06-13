document.addEventListener('DOMContentLoaded', () => {
    const messagesTable = document.getElementById('messagesTable');


    function checkHiveConditions(hiveRef) {
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
            const messagesTable = document.getElementById('messagesTable');
            messagesTable.innerHTML = ''; // Clear existing table entries

            // Loop through all entries in the data object
            Object.entries(data).forEach(([key, entry]) => {
                const timestamp = new Date(Number(key) * 1000);
                const timeAgo = timeSince(timestamp);
                const messages = generateMessages(entry);

                messages.forEach(message => {
                    const updateElement = document.createElement('div');
                    updateElement.classList.add('update');
                    updateElement.innerHTML =
                        `
                        <div class="message">
                            <p><b>${message.title}</b>: ${message.body}</p>
                            <small class="text-muted">${timeAgo}</small>
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

                    // Add the message to the table
                    const tableRow = document.createElement('tr');
                    tableRow.id = `message-${key}`;
                    tableRow.innerHTML = `
                        <td><span class="material-symbols-outlined">${message.icon}</span></td>
                        <td>${message.hiveId}</td>
                        <td class="warning">${timeAgo}</td>
                        <td class="primary">${message.body}</td>
                    `;
                    messagesTable.appendChild(tableRow);
                });
            });
        });
    }

    function generateMessages(entry) {
        const messages = [];
        const idealTemp = 32;
        const highTemp = 35;
        const lowHumidity = 20;
        const highHumidity = 80;
        const weightLoss = 10;

        if (entry.temperature < idealTemp) {
            messages.push({
                hiveId: entry.hiveId,
                icon: 'emergency_heat',
                title: `Hive ${entry.hiveId}`,
                body: `Low temperature of ${entry.temperature}⁰C reported from the hive.`,
            });
        }
        if (entry.temperature > highTemp) {
            messages.push({
                hiveId: entry.hiveId,
                icon: 'emergency_heat',
                title: `Hive ${entry.hiveId}`,
                body: `High temperature of ${entry.temperature}⁰C reported from the hive.`,
            });
        }
        if (entry.humidity < lowHumidity) {
            messages.push({
                hiveId: entry.hiveId,
                icon: 'water_drop',
                title: `Hive ${entry.hiveId}`,
                body: `Low humidity of ${entry.humidity}% reported from the hive.`,
            });
        }
        if (entry.humidity > highHumidity) {
            messages.push({
                hiveId: entry.hiveId,
                icon: 'water_drop',
                title: `Hive ${entry.hiveId}`,
                body: `High humidity of ${entry.humidity}% reported from the hive.`,
            });
        }
        if (entry.weight < weightLoss) {
            messages.push({
                hiveId: entry.hiveId,
                icon: 'scale',
                title: `Hive ${entry.hiveId}`,
                body: `Weight loss detected in the hive.`,
            });
        }
        if (entry.feedBees) {
            messages.push({
                hiveId: entry.hiveId,
                icon: 'food_bank',
                title: `Hive ${entry.hiveId}`,
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

    // Example Firebase reference
    // Replace this with the actual reference from your Firebase setup
    const hiveRef = ref(database, 'UsersData/your_user_id/Hive-01');
    checkHiveConditions(hiveRef);
});
