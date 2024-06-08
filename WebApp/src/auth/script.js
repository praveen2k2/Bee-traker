import { initializeApp } from "https://www.gstatic.com/firebasejs/10.12.1/firebase-app.js";
import { getAuth, createUserWithEmailAndPassword, signInWithEmailAndPassword } from "https://www.gstatic.com/firebasejs/10.12.1/firebase-auth.js";
import { getDatabase, set, ref} from "https://www.gstatic.com/firebasejs/10.12.1/firebase-database.js";

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

const database = getDatabase(app);

const signUp = document.getElementById('sign-up');
signUp.addEventListener("click", function (event) {
    event.preventDefault()

    const registerEmail = document.getElementById('registerEmail').value;
    const registerPassword = document.getElementById('registerPassword').value;

    const auth = getAuth();
    createUserWithEmailAndPassword(auth, registerEmail, registerPassword)
        .then((userCredential) => {
            // Signed up 
            const user = userCredential.user;

            set(ref(database, 'UsersData/' + user.uid + '/ProfileInfo'), {
                email: registerEmail,
            })

            alert("User Created. Sign-in to your account")
        })
        .catch((error) => {
            const errorCode = error.code;
            const errorMessage = error.message;
            alert(errorMessage)
            // ..
        });
})

const signIn = document.getElementById('sign-in');
signIn.addEventListener("click", function (event) {
  event.preventDefault();

  const loginEmail = document.getElementById('loginEmail').value;
  const loginPassword = document.getElementById('loginPassword').value;

  const auth = getAuth();
  signInWithEmailAndPassword(auth, loginEmail, loginPassword)
    .then((userCredential) => {
      // Signed in successfully
      const user = userCredential.user;
      console.log("User signed in:", user);

      // Redirect to the user's dashboard after successful login
      window.location.replace('../dashboard/'); // Use replace to prevent back button issues

    })
    .catch((error) => {
      const errorCode = error.code;
      const errorMessage = error.message;
      console.error("Login failed:", errorCode, errorMessage);

      // Handle login errors appropriately, e.g., display error messages
      alert(errorMessage);
    });
});
