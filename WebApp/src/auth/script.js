import { getAuth, createUserWithEmailAndPassword, signInWithEmailAndPassword  } from "https://www.gstatic.com/firebasejs/10.12.1/firebase-auth.js";


document.addEventListener("DOMContentLoaded", function () {
    const loginBtn = document.getElementById("loginBtn");
    const registerBtn = document.getElementById("registerBtn");
    const loginForm = document.getElementById("loginForm");
    const registerForm = document.getElementById("registerForm");

    // Initially, show the login form and hide the register form
    loginForm.style.display = "block";
    registerForm.style.display = "none";

    // Toggle between login and registration forms
    loginBtn.addEventListener("click", function () {
        loginForm.style.display = "block";
        registerForm.style.display = "none";
        loginBtn.classList.add("active");
        registerBtn.classList.remove("active");
    });

    registerBtn.addEventListener("click", function () {
        loginForm.style.display = "none";
        registerForm.style.display = "block";
        loginBtn.classList.remove("active");
        registerBtn.classList.add("active");
    });
});


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
            alert("Creating User...")
            window.location.herf = "index.html"
            // ...
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
      window.location.replace('../'); // Use replace to prevent back button issues

    })
    .catch((error) => {
      const errorCode = error.code;
      const errorMessage = error.message;
      console.error("Login failed:", errorCode, errorMessage);

      // Handle login errors appropriately, e.g., display error messages
      alert(errorMessage);
    });
});
